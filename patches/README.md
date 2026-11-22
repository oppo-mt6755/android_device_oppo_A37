# Platform changes required by this kernel/userspace combination

Apply these patches to the original LineageOS 15.1 project baselines using
`git apply --check` followed by `git apply`. Each directory is relative to the
Android source root. Do not apply the former common tree's entire patch series.

- system/core: compile-time option for the kernel's `/dev/android_adb` USB
  character device, instead of FunctionFS. Required to keep USB ADB available.
- external/tinycompress: isolate exported sound UAPI headers. The original
  makefile incorrectly injects ARM64 signal headers into the 32-bit build;
  the fresh build reproduced the `__uint128_t` compilation failure.
- frameworks/base: allow `/dev/mali0` and `/proc/ged` descriptors retained
  by the selected OPPO GPU driver during zygote EGL preload.

- hardware/interfaces: allow the audio service init script to be supplied by
  the device tree. The retained calibration files require the system group.

- hardware/interfaces: support the legacy sensor 1.1 interface without reading
  callbacks added by newer versions; normalize legacy sensor flags. Allow board
  service init files for the Wi-Fi, Bluetooth and sensor daemons. Send a full
  H4 packet with one write on character transports.
- system/bt: allow limiting extended feature pages for a controller that rejects
  page 2 with HCI status 0x30. Enabled only by ro.bluetooth.max_feature_page.

- system/connectivity/wificond: derive scan timestamps from local BSS age
  when the legacy kernel omits LAST_SEEN_BOOTTIME. Remote AP TSF clocks must
  not be compared with Android elapsed time. Includes age and precedence tests.

IMS integration lives in the device tree and requires no additional framework
patches.

- frameworks/av: Hermes MTK camera parameters and extended message handling for
  the HAL1 camera BSP. The graphics signatures are supplied by the device shim;
  no private framework or graphics runtime is copied into vendor.

- hardware/ril: let the board provide the Hermes MTK Oreo daemon instead of
  building a second generic rild service. The adapter is under radio/.
- system/core/healthd: restore the LED-class backlight after framebuffer resume
  in charger mode. The path and level are selected by the board configuration.

- hardware/interfaces camera HAL1 adapter: preserve a camera-specific allocation
  cookie when MTK passes null; supply real platform GraphicBuffers around HIDL
  handles so the legacy DisplayClient can use the NativeWindowBuffer C ABI.
  Both paths are enabled only by TARGET_HAS_LEGACY_CAMERA_HAL1.
- frameworks/av mediaserver: allow a board init file so the in-process HAL1
  camera can read system-owned calibration data and access graphics devices.
