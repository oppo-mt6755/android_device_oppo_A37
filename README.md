# OPPO A37 — LineageOS 17.1 bring-up

This branch starts from the verified LineageOS 15.1 device, vendor and kernel
commits:

- Device: `0e32eaf0e69ee43ee8c94776227832790a67c5fd`
- Vendor: `17ecf722aea49434b1590db1511ac8b26c9873e3`
- Kernel: `f462efae12d4701d5a4aa32461dfedda0f214d76`

The original 8.1 checkout and installed image remain the rollback baseline.

Initial target: build Android 10, preserve USB ADB, boot with the existing kernel
and display/touch drivers, then validate each hardware subsystem on the phone.

The partition layout remains non-Treble with vendor under system/vendor and
flattened APEX. First-stage init retains the root ramdisk after mounting system.
The kernel DT fstab already mounts system by its controller by-name path.
The first boot image passes a ramdisk content check; SELinux neverallow,
file-type and compatibility checks pass. Phone boot is still unverified.

The old MTK camera and encoder require a 160-byte GraphicBuffer allocation on
Android 10. Guarded blob instruction changes and DT_NEEDED additions are listed
in vendor/oppo/A37/shim-dependencies.json; the UI shim asserts the expected size.
Runtime camera functionality is not yet verified on Android 10.

Build: `device/oppo/A37/tools/build-android.sh <targets>` (userdebug, JOBS=8 by
default). The platform selects its bundled JDK; tools/a37-host/bin/m4 supplies
the missing host executable used by flex. tools/a37-host/lib contains ncurses5
compatibility libraries for the bundled RenderScript compiler. See patches/ for Android 10 platform
changes. SELinux remains permissive during bring-up.

The synced arm64 WebView main branch currently contains an unavailable LFS
object. Initial bring-up pins the official prebuilt to commit
`1411b097536a2f2c8188730a786ae08ef613195e` (109.0.5414.118, minSdk 24)
using .repo/local_manifests/a37-webview.xml. Update this component before
shipping a general-use build.
