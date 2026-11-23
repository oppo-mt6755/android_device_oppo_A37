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
flattened APEX. Android 10 packages the root directory in the system image;
its standard first-stage init mounts that image and switches root before
starting /system/bin/init. Boot contains only the first-stage ramdisk.
The kernel DT fstab already mounts system by its controller by-name path.
SELinux neverallow, file-type and compatibility checks pass.
Android 10 completes boot and enters the system. The initial storage-mount
panic is fixed; normal reboot and Binder/storage regression checks pass.
Other hardware subsystems still need Android 10 runtime validation. Logd, JIT and primary
audio now start after ambient-capability, memfd and legacy audio-layout fixes.
The stock audio HAL perf-service hook is bypassed: its pre-Android-10 Parcel
allocation overwrites caller state when used with the current libbinder.
The kernel also carries upstream 54708d2858e7 so capability-bearing services
can resolve their own /proc/self/fd from worker threads.
The audio policy routes UI effects through primary DL1 output: the stock
kernel has no MultiMedia2_PLayback PCM for the HAL fast-output handler.

LiveDisplay owns day/night color temperature through the existing color-matrix
backend. The separate Android Night Display is disabled so it does not suppress
Lineage's temperature controller. Day/night switching and display wake have
passed user testing.

The old MTK camera and encoder require a 160-byte GraphicBuffer allocation on
Android 10. Guarded blob instruction changes and DT_NEEDED additions are listed
in vendor/oppo/A37/shim-dependencies.json; the UI shim asserts the expected size.
Snap is the default camera. Front/rear preview and photos work; HDR, video
recording, saving and playback have passed user testing. A subsequent GPU
cleanup crash was fixed by auditing all GraphicBuffer allocations; seven
alternating recording/decode/release probes then passed without a media-service
restart. Extended app testing continues.

Wi-Fi scanning/connection and Bluetooth pairing have passed. The kernel carries
upstream neighbor-state notification and probe-reset fixes (53385d2 and 765c9c6,
Android common 2011e72). Without resetting the count, repeated NUD_PROBE requests
can make Android disconnect a working Wi-Fi connection. With GPS Test active,
24 rounds of IPv4/IPv6 neighbor probes passed without a disconnect; longer
real-world Wi-Fi stability still needs observation.

The radio reads
USIM records and attaches to LTE. IMS registration, outgoing calls, hangup,
two-way audio and dial keypad sound have passed user testing after adapting the Q
call-session listener and legacy audio port layouts. Incoming calls, SMS send and
receive, and reopening Snap after recording have also passed user testing.
Native RIL Parcel allocations and SIM manager buffer initialization
are guarded in the same vendor fixup manifest. The legacy IMS app adapts Q data
state and IMS parcelable APIs locally, without changing framework Java APIs.

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

Kernel memfd currently supports anonymous shared mappings and CLOEXEC, as
needed by ART. MFD_ALLOW_SEALING is rejected until file sealing is backported.

Binder and sdcardfs now use the A6010 lineage-17.1 implementation at
`f7d8195672d653c66eff51066123f46977100f86`, adapted for the retained ARM64
OPPO kernel. See kernel Documentation/android.txt for provenance and differences.
The VFS mount failure path must free per-mount data only once: unsupported
sdcardfs mount options previously triggered a double free and kernel panic.
