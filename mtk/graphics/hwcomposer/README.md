# MediaTek framebuffer composer

The MTK gralloc module dispatches only `gpu0` and `extra`. It still exports
`framebuffer_device_open`, used here to obtain the actual framebuffer metrics.
Its `fb_post` logs that ION_FB_HEAP presentation is unimplemented and shows only
the first frame. The source HWC now obtains the ION fd with public gralloc_extra query 1,
imports it through `/dev/mtk_disp_mgr` PREPARE_INPUT_BUFFER and submits
FRAME_CONFIG, using the selected target kernel’s disp_session.h.
The display reads the GPU-composed client target directly. A checked NEON
framebuffer copy + FBIOPAN_DISPLAY remains a fallback if import is unavailable.
Opening HWC preserves the inherited framebuffer mode and the LK splash colors.
The direct ION path selects each submitted target's format and needs no fbdev
mode change. Only the copy fallback switches to RGBA, after copying its first
complete frame into the page selected by that same mode change.
`FBIOPUT_VSCREENINFO` clears the vendor kernel's
`FB_ACTIVATE_NO_UPDATE` state before the first pan. It must set
`FB_ACTIVATE_FORCE`: the kernel reports activate=NOW even while its private
no_update flag is set, so an otherwise identical mode would skip fb_check_var
and silently ignore all pans. Hardware trigger counts and user-visible panel
output confirmed this on the tested target.

The code queries buffer dimensions, stride and format through the public
`gralloc_extra_query` C ABI, rather than assuming a private native-handle layout.
Query constants 10/11/12/15 are defined in MediaTek's published header:
https://github.com/archermind/MTK_X20_BASE_AOSP/blob/master/vendor/amt/libs/mt6797_64/libgralloc_extra/include/ui/gralloc_extra.h
They are validated against the live target buffer metadata before copying.

The driver-provided input release fence is returned as the HWC1 retire fence
and duplicated for the client target release fence. HWC2On1Adapter delays this
by one frame to implement HWC2 present semantics. GPU acquire fences are waited
before submission. No fake signaled fence substitutes for buffer ownership.
Vsync uses the MTK display IRQ timestamp ioctl, with paced software fallback;
the kernel wait is bounded to handle panel shutdown. SurfaceFlinger sync
framework support is enabled.

The submission/lifetime design was compared with MSM8916's mature implementation:
https://github.com/LineageOS/android_hardware_qcom_display/blob/lineage-15.1-caf-8916/libhwcomposer/hwc_fbupdate.cpp
https://github.com/LineageOS/android_hardware_qcom_display/blob/lineage-15.1-caf-8916/libhwcomposer/hwc_utils.cpp
https://github.com/LineageOS/android_hardware_qcom_display/blob/lineage-15.1-caf-8916/libhwcomposer/hwc_vsync.cpp
Qualcomm's ioctls are not used on MTK. In particular, the old MTK primary
SET_INPUT_BUFFER ioctl calls BUG(); only the current FRAME_CONFIG API is used.

Measured copy time was 37–40 ms per frame with libc memcpy, 19–21 ms with NEON,
and no copy is needed for the direct path (0.4–0.6 ms submission). Settings scroll
frame median improved from 109 ms to 16 ms in short controlled runs. The user
confirmed correct colors and visibly smoother scrolling. This is not a claim
of constant 60 FPS or optimal power consumption. Layers are still GPU composed;
secure/multilayer overlay composition has not been implemented. Stride, format,
fence and fallback framebuffer allocation checks are retained.
