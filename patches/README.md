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

No IMS, RIL, camera, Bluetooth or Wi-Fi platform patches are enabled at this stage.
