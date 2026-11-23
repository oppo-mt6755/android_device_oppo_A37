# LineageOS 17.1 platform patches

Apply each patch from its corresponding source project using `git apply`.
The patches were checked against these LineageOS 17.1 commits:

- `system/core`: `6d27634866be5b646a8cea1f9abeae902fd831e5`
- `hardware/interfaces`: `02951860e9c95f19384fa07522925537d8c17a18`
- `frameworks/av`: `c2725f53d8e9d6dfa5b853124d21249d2db106da`
- `frameworks/base`: `2fd409fbf482fb0ec2bc9507dd9daa8b773fb40d`
- `external/tinyalsa`: `b4b3a6d4dd962994e595ae6f8fcdcb5d77a3c6b0`
- `external/tinycompress`: `848ec3ad67cc414294d18776a2b4d644be95fd64`
- `system/bt`: `a15831c01596b53d57482812678bcc8276f3c401`
- `system/connectivity/wificond`: `b680ee54032549388cf5d7b9a2d076241c2f6f10`
- `vendor/lineage`: `e85694867b50377d7cfc82edd7417dde0b5ac386`

- `system/sepolicy`: `b052c214f3ca11d2bbe09b6d34e0ce693fdc8d92`

- `device/lineage/sepolicy`: `72049da6bc559ad5e57a2824b975fba5102258a0`

- `hardware/libhardware`: `8caddb38009ef6f4fd650795637891698e34bb3d`

Changes:

- system/core: character-device USB ADB;
  restore charger LED backlight after panel resume.
- frameworks/base: retain Mali/GED descriptors during zygote EGL preload.
- frameworks/av: legacy MTK camera parameters/messages and board service init.
- hardware/interfaces: HAL1 buffers, sensor 1.1, NMEA termination,
  complete H4 character-device writes, optional audio debug callbacks and board service init.
- system/sepolicy and device/lineage/sepolicy: register the in-process HAL1
  service through the existing camera-service attribute without conflicting
  exclusive registration rules.
- hardware/libhardware: opt-in legacy audio device ABI for HAL 2.0.
- vendor/lineage: board service-init/audio ABI options and China Unicom IMS APN.
- external/tinyalsa: validate absent PCM handles before timestamp queries.
- external/tinycompress: export only the legacy sound UAPI headers.
- system/bt: limit feature queries for the older Bluetooth controller.
- system/connectivity/wificond: convert local BSS age to boot-relative time.

Compile validation passes. Android 10 starts its boot animation and SystemUI;
full boot completion and hardware acceptance are still in progress. The earlier Android 8.1 patch set remains in the original
LineageOS 15.1 device branch.
