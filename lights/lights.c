/* SPDX-License-Identifier: Apache-2.0 */
#define LOG_TAG "lights"
#include <hardware/lights.h>
#include <log/log.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char brightness_path[] = "/sys/class/leds/lcd-backlight/brightness";

static int set_backlight(struct light_device_t *device, const struct light_state_t *state)
{
    (void)device;
    if (!state) return -EINVAL;
    if (state->brightnessMode == BRIGHTNESS_MODE_LOW_PERSISTENCE) return -ENOSYS;
    unsigned int color = state->color;
    unsigned int level = (77 * ((color >> 16) & 255) +
                          150 * ((color >> 8) & 255) + 29 * (color & 255)) >> 8;
    char value[16];
    int length = snprintf(value, sizeof(value), "%u\n", level);
    int fd = TEMP_FAILURE_RETRY(open(brightness_path, O_WRONLY | O_CLOEXEC));
    if (fd < 0) {
        int error = errno;
        ALOGE("Cannot open backlight: %s", strerror(error));
        return -error;
    }
    ssize_t result = TEMP_FAILURE_RETRY(write(fd, value, length));
    int error = result < 0 ? errno : EIO;
    close(fd);
    if (result != length) {
        ALOGE("Cannot set backlight to %u: %s", level, strerror(error));
        return -error;
    }
    return 0;
}

static int close_light(struct hw_device_t *device)
{
    free(device);
    return 0;
}

static int open_light(const struct hw_module_t *module, const char *name,
                      struct hw_device_t **result)
{
    if (!name || !result) return -EINVAL;
    if (strcmp(name, LIGHT_ID_BACKLIGHT)) return -ENOSYS;
    struct light_device_t *device = calloc(1, sizeof(*device));
    if (!device) return -ENOMEM;
    device->common.tag = HARDWARE_DEVICE_TAG;
    device->common.version = LIGHTS_DEVICE_API_VERSION_2_0;
    device->common.module = (struct hw_module_t *)module;
    device->common.close = close_light;
    device->set_light = set_backlight;
    *result = &device->common;
    return 0;
}

static struct hw_module_methods_t methods = { .open = open_light };
struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .module_api_version = LIGHTS_HEADER_VERSION,
    .hal_api_version = HARDWARE_HAL_API_VERSION,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "MTK sysfs backlight",
    .author = "LineageOS",
    .methods = &methods,
};
