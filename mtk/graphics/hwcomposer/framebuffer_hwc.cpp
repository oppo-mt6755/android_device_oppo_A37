/*
 * Copyright (C) 2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 * Client-composition HWC 1.1 for the legacy MediaTek BSP. Uses the stock gralloc
 * framebuffer ABI; avoids the Lollipop-only GuiExt/BufferQueue interfaces.
 */
#define LOG_TAG "MtkFramebufferHWC"
#include <hardware/fb.h>
#include <hardware/gralloc.h>
#include <hardware/hwcomposer.h>
#include <log/log.h>
#include <sync/sync.h>
#include <errno.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <atomic>
#include <new>
#include <arm_neon.h>
#include "disp_session.h"

struct Composer {
    hwc_composer_device_1_t device{};
    framebuffer_device_t* fb = nullptr;
    const gralloc_module_t* gralloc = nullptr;
    int fbFd = -1;
    int displayFd = -1;
    int vsyncFd = -1;
    disp_session_config session{};
    bool loggedDirect = false;
    bool loggedVsync = false;
    unsigned directFrames = 0;
    int64_t directNs = 0;
    fb_var_screeninfo var{};
    fb_fix_screeninfo fix{};
    uint8_t* memory = nullptr;
    size_t memorySize = 0;
    unsigned page = 0;
    unsigned pages = 1;
    void* extraLibrary = nullptr;
    using Query = int (*)(buffer_handle_t, int, void*);
    Query query = nullptr;
    bool loggedBuffer = false;
    bool fbModeReady = false;
    unsigned profileFrames = 0;
    int64_t profileNs[5]{};
    pthread_t thread{};
    std::atomic<bool> stopping{false};
    std::atomic<bool> vsyncEnabled{false};
    std::atomic<bool> screenOn{true};
    std::atomic<const hwc_procs_t*> callbacks{nullptr};
    int64_t period = 16666667;
};

static Composer* context(hwc_composer_device_1_t* device) {
    return reinterpret_cast<Composer*>(device);
}

static int64_t monotonicNs() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return static_cast<int64_t>(now.tv_sec) * 1000000000 + now.tv_nsec;
}

static void copyDisplayRow(uint8_t* destination, const uint8_t* source, size_t count) {
    while (count >= 64) {
        const uint8x16_t a = vld1q_u8(source);
        const uint8x16_t b = vld1q_u8(source + 16);
        const uint8x16_t c = vld1q_u8(source + 32);
        const uint8x16_t d = vld1q_u8(source + 48);
        vst1q_u8(destination, a);
        vst1q_u8(destination + 16, b);
        vst1q_u8(destination + 32, c);
        vst1q_u8(destination + 48, d);
        source += 64;
        destination += 64;
        count -= 64;
    }
    if (count) memcpy(destination, source, count);
}

static void* vsyncThread(void* data) {
    auto* self = static_cast<Composer*>(data);
    int64_t previous = 0;
    while (!self->stopping.load()) {
        int64_t timestamp = 0;
        if (self->vsyncFd >= 0 && self->vsyncEnabled.load() && self->screenOn.load()) {
            disp_session_vsync_config event{};
            event.session_id = self->session.session_id;
            if (!ioctl(self->vsyncFd, DISP_IOCTL_WAIT_FOR_VSYNC, &event)) {
                const int64_t now = monotonicNs();
                const int64_t hardwareTimestamp = static_cast<int64_t>(event.vsync_ts);
                if (hardwareTimestamp > previous && hardwareTimestamp <= now &&
                        now - hardwareTimestamp < 1000000000LL) {
                    timestamp = hardwareTimestamp;
                    if (!self->loggedVsync) {
                        ALOGI("using display IRQ vsync timestamps");
                        self->loggedVsync = true;
                    }
                }
            }
        }
        if (!timestamp) {
            struct timespec delay{0, static_cast<long>(self->period)};
            while (nanosleep(&delay, &delay) < 0 && errno == EINTR) {}
            timestamp = monotonicNs();
        }
        const auto* callbacks = self->callbacks.load();
        if (callbacks && callbacks->vsync && self->vsyncEnabled.load() && self->screenOn.load()) {
            callbacks->vsync(callbacks, HWC_DISPLAY_PRIMARY, timestamp);
            previous = timestamp;
        }
    }
    return nullptr;
}

static int prepare(hwc_composer_device_1_t*, size_t count, hwc_display_contents_1_t** displays) {
    for (size_t d = 0; displays && d < count; ++d) {
        auto* display = displays[d];
        if (!display) continue;
        for (size_t i = 0; i < display->numHwLayers; ++i) {
            auto& layer = display->hwLayers[i];
            if (layer.compositionType != HWC_FRAMEBUFFER_TARGET) layer.compositionType = HWC_FRAMEBUFFER;
        }
    }
    return 0;
}

// MTK's exported fb_post does not implement ION_FB_HEAP presentation. Copy
// the GPU-composed target through gralloc's mapping API, then trigger the real
// framebuffer pan operation. Never interpret the private native-handle layout.
// Query IDs 10/11/12/15 are the public MTK gralloc_extra width/height/stride/format
// ABI, documented in hwcomposer/README.md.
static int postDirect(Composer* self, buffer_handle_t buffer, int width, int height,
        int stride, int format, int* releaseFence) {
    int ionFd = -1;
    if (self->query(buffer, 1, &ionFd) || ionFd < 0) return -EINVAL;
    disp_buffer_info input{};
    input.session_id = self->session.session_id;
    input.layer_id = 0;
    input.layer_en = 1;
    input.ion_fd = ionFd;
    input.fence_fd = -1;
    const int64_t start = monotonicNs();
    if (ioctl(self->displayFd, DISP_IOCTL_PREPARE_INPUT_BUFFER, &input) < 0)
        return -errno;
    if (input.fence_fd < 0 || !input.index) return -EIO;
    disp_frame_cfg_t frame{};
    frame.setter = frame.user = SESSION_USER_HWC;
    frame.session_id = self->session.session_id;
    frame.mode = DISP_SESSION_DIRECT_LINK_MODE;
    frame.present_fence_idx = static_cast<unsigned>(-1);
    frame.input_layer_num = 4;
    frame.overlap_layer_num = 1;
    for (unsigned i = 0; i < frame.input_layer_num; ++i) frame.input_cfg[i].layer_id = i;
    auto& layer = frame.input_cfg[0];
    layer.layer_enable = 1;
    layer.buffer_source = DISP_BUFFER_ION;
    layer.src_fmt = format == HAL_PIXEL_FORMAT_BGRA_8888 ? DISP_FORMAT_BGRA8888 :
            format == HAL_PIXEL_FORMAT_RGBX_8888 ? DISP_FORMAT_RGBX8888 : DISP_FORMAT_RGBA8888;
    layer.src_pitch = stride;
    layer.src_width = layer.tgt_width = width;
    layer.src_height = layer.tgt_height = height;
    layer.next_buff_idx = input.index;
    layer.alpha = 255;
    if (ioctl(self->displayFd, DISP_IOCTL_FRAME_CONFIG, &frame) < 0) {
        const int error = -errno;
        close(input.fence_fd);
        return error;
    }
    // This is a HWC1 retire fence: it signals when this target is replaced.
    // HWC2On1Adapter defers it by one frame for HWC2 present-fence semantics.
    *releaseFence = input.fence_fd;
    if (!self->loggedDirect) {
        ALOGI("direct ION scanout: %dx%d stride=%d format=%d session=%x index=%u",
                width, height, stride, format, frame.session_id, input.index);
        self->loggedDirect = true;
    }
    self->directNs += monotonicNs() - start;
    if (++self->directFrames == 120) {
        ALOGI("direct submit average ms: %.3f", self->directNs / 120000000.0);
        self->directFrames = 0;
        self->directNs = 0;
    }
    return 0;
}

static int postTarget(Composer* self, buffer_handle_t buffer, int* releaseFence) {
    int width = 0, height = 0, stride = 0, format = 0;
    if (self->query(buffer, 10, &width) || self->query(buffer, 11, &height) ||
            self->query(buffer, 12, &stride) || self->query(buffer, 15, &format)) return -EINVAL;
    if (width != static_cast<int>(self->var.xres) || height != static_cast<int>(self->var.yres) ||
            stride < width || stride > 16384) return -EINVAL;
    if (format != HAL_PIXEL_FORMAT_RGBA_8888 && format != HAL_PIXEL_FORMAT_RGBX_8888 &&
            format != HAL_PIXEL_FORMAT_BGRA_8888) return -ENOTSUP;
    if (self->displayFd >= 0) {
        int directError = postDirect(self, buffer, width, height, stride, format, releaseFence);
        if (!directError) return 0;
        ALOGE("direct scanout failed: %d; using framebuffer copies", directError);
        close(self->displayFd);
        self->displayFd = -1;
    }
    void* mapped = nullptr;
    const int64_t start = monotonicNs();
    int error = self->gralloc->lock(self->gralloc, buffer, GRALLOC_USAGE_SW_READ_RARELY,
            0, 0, width, height, &mapped);
    const int64_t locked = monotonicNs();
    if (error) return error;
    if (!mapped) {
        self->gralloc->unlock(self->gralloc, buffer);
        return -EFAULT;
    }
    if (!self->loggedBuffer) {
        ALOGI("copying %dx%d target: stride=%d format=%d, fb stride=%u pages=%u",
                width, height, stride, format, self->fix.line_length, self->pages);
        self->loggedBuffer = true;
    }
    const auto* source = static_cast<const uint8_t*>(mapped);
    unsigned page = self->page++ % self->pages;
    uint8_t* destination = self->memory + page * self->var.yres * self->fix.line_length;
    for (int y = 0; y < height; ++y) {
        const uint8_t* row = source + static_cast<size_t>(y) * stride * 4;
        uint8_t* output = destination + static_cast<size_t>(y) * self->fix.line_length;
        if (format != HAL_PIXEL_FORMAT_BGRA_8888) {
            copyDisplayRow(output, row, static_cast<size_t>(width) * 4);
        } else {
            for (int x = 0; x < width; ++x) {
                output[x*4] = row[x*4+2];
                output[x*4+1] = row[x*4+1];
                output[x*4+2] = row[x*4];
                output[x*4+3] = row[x*4+3];
            }
        }
    }
    const int64_t copied = monotonicNs();
    error = self->gralloc->unlock(self->gralloc, buffer);
    const int64_t unlocked = monotonicNs();
    if (error) return error;
    self->var.yoffset = page * self->var.yres;
    if (!self->fbModeReady) {
        // Preserve LK's channel order until a complete replacement frame is in
        // this page. Direct ION scanout does not need a fbdev mode change.
        self->var.red = {0, 8, 0};
        self->var.green = {8, 8, 0};
        self->var.blue = {16, 8, 0};
        self->var.transp = {24, 8, 0};
        // FORCE clears MTK's hidden no_update flag even for an identical mode.
        self->var.activate = FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
        if (ioctl(self->fbFd, FBIOPUT_VSCREENINFO, &self->var) < 0) return -errno;
        self->fbModeReady = true;
    }
    self->var.activate = FB_ACTIVATE_VBL;
    error = ioctl(self->fbFd, FBIOPAN_DISPLAY, &self->var) < 0 ? -errno : 0;
    const int64_t posted = monotonicNs();
    self->profileNs[0] += locked - start;
    self->profileNs[1] += copied - locked;
    self->profileNs[2] += unlocked - copied;
    self->profileNs[3] += posted - unlocked;
    if (++self->profileFrames == 60) {
        ALOGI("present average ms: lock=%.2f copy=%.2f unlock=%.2f pan=%.2f fence=%.2f",
                self->profileNs[0] / 60000000.0, self->profileNs[1] / 60000000.0,
                self->profileNs[2] / 60000000.0, self->profileNs[3] / 60000000.0,
                self->profileNs[4] / 60000000.0);
        self->profileFrames = 0;
        memset(self->profileNs, 0, sizeof(self->profileNs));
    }
    return error;
}

static int present(hwc_composer_device_1_t* device, size_t count,
        hwc_display_contents_1_t** displays) {
    auto* self = context(device);
    int result = 0;
    for (size_t d = 0; displays && d < count; ++d) {
        auto* display = displays[d];
        if (!display) continue;
        display->retireFenceFd = -1;
        for (size_t i = 0; i < display->numHwLayers; ++i) {
            auto& layer = display->hwLayers[i];
            layer.releaseFenceFd = -1;
            int waitResult = 0;
            const int64_t waitStart = monotonicNs();
            if (layer.acquireFenceFd >= 0) {
                waitResult = sync_wait(layer.acquireFenceFd, 3000);
                if (waitResult < 0 && result == 0) result = -errno;
                close(layer.acquireFenceFd);
                layer.acquireFenceFd = -1;
            }
            if (d == HWC_DISPLAY_PRIMARY && waitResult == 0 && self->screenOn.load() &&
                    layer.compositionType == HWC_FRAMEBUFFER_TARGET && layer.handle) {
                self->profileNs[4] += monotonicNs() - waitStart;
                int error = postTarget(self, layer.handle, &display->retireFenceFd);
                if (display->retireFenceFd >= 0) layer.releaseFenceFd = dup(display->retireFenceFd);
                if (error && result == 0) result = error;
            }
        }
    }
    if (result) ALOGE("framebuffer present failed: %d", result);
    return result;
}

static int eventControl(hwc_composer_device_1_t* device, int display, int event, int enabled) {
    if (display != HWC_DISPLAY_PRIMARY || event != HWC_EVENT_VSYNC || (enabled != 0 && enabled != 1))
        return -EINVAL;
    context(device)->vsyncEnabled.store(enabled != 0);
    return 0;
}

static int blank(hwc_composer_device_1_t* device, int display, int blanked) {
    if (display != HWC_DISPLAY_PRIMARY) return -EINVAL;
    auto* self = context(device);
    int result = ioctl(self->fbFd, FBIOBLANK, blanked ? FB_BLANK_POWERDOWN : FB_BLANK_UNBLANK) < 0 ? -errno : 0;
    if (!result) self->screenOn.store(!blanked);
    return result;
}

static int query(hwc_composer_device_1_t* device, int what, int* value) {
    if (!value) return -EINVAL;
    switch (what) {
        case HWC_BACKGROUND_LAYER_SUPPORTED: *value = 0; return 0;
        case HWC_VSYNC_PERIOD: *value = context(device)->period; return 0;
        case HWC_DISPLAY_TYPES_SUPPORTED: *value = HWC_DISPLAY_PRIMARY_BIT; return 0;
        default: return -EINVAL;
    }
}

static void registerCallbacks(hwc_composer_device_1_t* device, const hwc_procs_t* callbacks) {
    context(device)->callbacks.store(callbacks);
}

static int configurations(hwc_composer_device_1_t*, int display, uint32_t* configs, size_t* count) {
    if (display != HWC_DISPLAY_PRIMARY || !count) return -EINVAL;
    if (configs && *count) configs[0] = 0;
    *count = 1;
    return 0;
}

static int attributes(hwc_composer_device_1_t* device, int display, uint32_t config,
        const uint32_t* attributes, int32_t* values) {
    if (display != HWC_DISPLAY_PRIMARY || config != 0 || !attributes || !values) return -EINVAL;
    auto* self = context(device);
    for (size_t i = 0; attributes[i] != HWC_DISPLAY_NO_ATTRIBUTE; ++i) {
        switch (attributes[i]) {
            case HWC_DISPLAY_VSYNC_PERIOD: values[i] = self->period; break;
            case HWC_DISPLAY_WIDTH: values[i] = self->fb->width; break;
            case HWC_DISPLAY_HEIGHT: values[i] = self->fb->height; break;
            case HWC_DISPLAY_DPI_X: values[i] = self->fb->xdpi * 1000; break;
            case HWC_DISPLAY_DPI_Y: values[i] = self->fb->ydpi * 1000; break;
            default: return -EINVAL;
        }
    }
    return 0;
}

static int closeDevice(hw_device_t* device) {
    auto* self = reinterpret_cast<Composer*>(device);
    self->stopping.store(true);
    pthread_join(self->thread, nullptr);
    if (self->memory) munmap(self->memory, self->memorySize);
    if (self->fbFd >= 0) close(self->fbFd);
    if (self->displayFd >= 0) close(self->displayFd);
    if (self->vsyncFd >= 0) close(self->vsyncFd);
    if (self->extraLibrary) dlclose(self->extraLibrary);
    framebuffer_close(self->fb);
    delete self;
    return 0;
}

static int openDevice(const hw_module_t* module, const char* name, hw_device_t** device) {
    if (!name || strcmp(name, HWC_HARDWARE_COMPOSER) != 0 || !device) return -EINVAL;
    const hw_module_t* gralloc = nullptr;
    int error = hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &gralloc);
    if (error) return error;
    auto* self = new (std::nothrow) Composer;
    if (!self) return -ENOMEM;
    auto& hwc = self->device;
    error = framebuffer_open(gralloc, &self->fb);
    // This MTK module exports its framebuffer implementation but its open()
    // dispatcher only accepts "gpu0" and "extra". Use that same implementation
    // directly rather than treating a missing dispatcher entry as missing HW.
    if (error == -EINVAL && gralloc->dso) {
        using FramebufferOpen = int (*)(const hw_module_t*, const char*, hw_device_t**);
        auto openFramebuffer = reinterpret_cast<FramebufferOpen>(dlsym(gralloc->dso,
                "_Z23framebuffer_device_openPK11hw_module_tPKcPP11hw_device_t"));
        if (openFramebuffer) {
            error = openFramebuffer(gralloc, GRALLOC_HARDWARE_FB0,
                    reinterpret_cast<hw_device_t**>(&self->fb));
            ALOGI("MTK framebuffer entry point returned %d", error);
        }
    }
    if (error || !self->fb || !self->fb->post) {
        ALOGE("cannot open gralloc framebuffer: %d", error);
        if (self->fb) framebuffer_close(self->fb);
        delete self;
        return error ? error : -ENODEV;
    }
    self->gralloc = reinterpret_cast<const gralloc_module_t*>(gralloc);
    self->extraLibrary = dlopen("libgralloc_extra.so", RTLD_NOW | RTLD_LOCAL);
    if (self->extraLibrary) self->query = reinterpret_cast<Composer::Query>(
            dlsym(self->extraLibrary, "gralloc_extra_query"));
    self->fbFd = open("/dev/graphics/fb0", O_RDWR | O_CLOEXEC);
    error = -ENODEV;
    if (!self->query || self->fbFd < 0 ||
            ioctl(self->fbFd, FBIOGET_VSCREENINFO, &self->var) < 0 ||
            ioctl(self->fbFd, FBIOGET_FSCREENINFO, &self->fix) < 0) goto framebuffer_error;
    // Stock gralloc supplies a 32-bit framebuffer. Keep its inherited mode
    // while the LK splash is visible; changing RGB/BGR here recolors the splash
    // before bootanimation has produced a replacement. Configure only when the
    // copy fallback submits its first complete frame.
    if (self->var.bits_per_pixel != 32) goto framebuffer_error;
    self->var.xoffset = self->var.yoffset = 0;
    self->memorySize = self->fix.smem_len;
    if (!self->var.yres || self->fix.line_length < self->var.xres * 4 ||
            self->memorySize < static_cast<size_t>(self->var.yres) * self->fix.line_length)
        goto framebuffer_error;
    self->pages = self->memorySize / (static_cast<size_t>(self->var.yres) * self->fix.line_length);
    if (self->pages > self->var.yres_virtual / self->var.yres)
        self->pages = self->var.yres_virtual / self->var.yres;
    if (self->pages > 2) self->pages = 2;
    if (!self->pages) goto framebuffer_error;
    // Gralloc keeps the boot splash on page zero; prepare the other page first.
    self->page = self->pages > 1 ? 1 : 0;
    self->memory = static_cast<uint8_t*>(mmap(nullptr, self->memorySize,
            PROT_READ | PROT_WRITE, MAP_SHARED, self->fbFd, 0));
    if (self->memory == MAP_FAILED) { self->memory = nullptr; goto framebuffer_error; }
    self->displayFd = open("/dev/mtk_disp_mgr", O_RDWR | O_CLOEXEC);
    if (self->displayFd >= 0) {
        self->session.type = DISP_SESSION_PRIMARY;
        self->session.mode = DISP_SESSION_DIRECT_LINK_MODE;
        self->session.user = SESSION_USER_HWC;
        if (ioctl(self->displayFd, DISP_IOCTL_CREATE_SESSION, &self->session) < 0 ||
                ioctl(self->displayFd, DISP_IOCTL_SET_SESSION_MODE, &self->session) < 0) {
            ALOGW("display session unavailable: %s", strerror(errno));
            close(self->displayFd);
            self->displayFd = -1;
        }
        if (self->displayFd >= 0)
            self->vsyncFd = open("/dev/mtk_disp_mgr", O_RDWR | O_CLOEXEC);
    }
    if (self->fb->fps > 1 && self->fb->fps < 240) self->period = 1000000000LL / self->fb->fps;
    hwc.common.tag = HARDWARE_DEVICE_TAG;
    hwc.common.version = HWC_DEVICE_API_VERSION_1_1;
    hwc.common.module = const_cast<hw_module_t*>(module);
    hwc.common.close = closeDevice;
    hwc.prepare = prepare;
    hwc.set = present;
    hwc.eventControl = eventControl;
    hwc.blank = blank;
    hwc.query = query;
    hwc.registerProcs = registerCallbacks;
    hwc.getDisplayConfigs = configurations;
    hwc.getDisplayAttributes = attributes;
    error = pthread_create(&self->thread, nullptr, vsyncThread, self);
    if (error) { error = -error; goto framebuffer_error; }
    ALOGI("client composition on %ux%u framebuffer, vsync period %lld ns",
            self->fb->width, self->fb->height, static_cast<long long>(self->period));
    *device = &hwc.common;
    return 0;
framebuffer_error:
    ALOGE("framebuffer copy setup failed: %d (%s)", error, strerror(errno));
    if (self->memory) munmap(self->memory, self->memorySize);
    if (self->fbFd >= 0) close(self->fbFd);
    if (self->displayFd >= 0) close(self->displayFd);
    if (self->vsyncFd >= 0) close(self->vsyncFd);
    if (self->extraLibrary) dlclose(self->extraLibrary);
    framebuffer_close(self->fb);
    delete self;
    return error;
}

static hw_module_methods_t methods = { .open = openDevice };
hwc_module_t HAL_MODULE_INFO_SYM = { .common = {
    .tag = HARDWARE_MODULE_TAG,
    .module_api_version = HWC_MODULE_API_VERSION_0_1,
    .hal_api_version = HARDWARE_HAL_API_VERSION,
    .id = HWC_HARDWARE_MODULE_ID,
    .name = "MTK LP framebuffer client composition",
    .author = "LineageOS",
    .methods = &methods,
    .dso = nullptr,
    .reserved = {},
} };
