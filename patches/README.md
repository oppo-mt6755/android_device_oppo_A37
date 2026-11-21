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

No IMS, RIL or camera platform patches are enabled at this stage.
