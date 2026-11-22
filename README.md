# OPPO A37 — SonyCustoms Oreo base

This device tree preserves the history of SonyCustoms/device_sony_tuba (oreo,
base c348e1a66e0b). Hardware implementations are grouped under mtk/, following the Hermes tree.
The first target is Android boot with display, touch and USB ADB. Other hardware
services are enabled only after checking the donor HAL against the target kernel.

Build with `tools/build-android.sh bootimage systemimage` from this directory
or invoke the script from the source root. The default output is
`out-sony-restart`, separate from the previous port. Android's init binary is
used directly. Unused donor shell-init/debug files and the disabled libtuba
module have been removed.

The kernel remains `kernel/oppo/mt6750`, configuration
`oppo6750_15127_debug_defconfig`. Its DT mounts system in first-stage init.
The real device has 16 MiB boot/recovery and a 2 GiB system partition.
Recovery uses its own fstab; the Android fstab does not remount system.

Vendor is maintained in `vendor/oppo/A37`, with all blobs under `proprietary/`
using paths relative to `/system`, following the Hermes layout. Its history
originates from SonyCustoms/vendor_sony_tuba. A37 firmware is installed only
at `/system/vendor/etc/firmware`. Sony r12 Mali requires kernel ABI 10.2,
whereas the retained OPPO driver implements ABI 8.0. The matching OPPO Mali,
GPU auxiliary library and gralloc replace those three donor components.
32-bit and 64-bit isolated EGLImage/pixel-readback probes pass with this mix.
The OPPO libstlport runtime is included because the stock GPU imports it.
The donor HWC frame structs also differ from the retained kernel; the source
composer in mtk/graphics/hwcomposer/ uses the actual kernel header. See the vendor's
firmware-provenance.json and graphics-provenance.json for exact sources.

The libshims/ modules supply imported legacy C, graphics, JPEG and native IMS
signatures, grouped like Hermes under misc/, ui/, jpeg/ and ims/. Only each
consumer declares its required shim through DT_NEEDED; no board linker
injection or global preload is used. The vendor fixup script and hash manifest
make the patchelf changes reproducible.
Necessary platform changes are exported under patches/ with their reasons.
SELinux is permissive during bring-up. Android reaches the Trebuchet desktop;
physical display and touch were confirmed on 2026-11-20. USB ADB also works.
Camera preview and JPEG capture are verified; long suspend remains under test.

The Synaptics touch driver follows primary framebuffer blank events. Oreo
autosleep and the display can bypass legacy early-suspend callbacks; relying
on those callbacks alone left the controller asleep with the screen on after
overnight standby. Six screen cycles and an explicit legacy sleep followed by
fbdev wake passed, with physical touch confirmed after two manual wake cycles.
An overnight retest remains pending.

Audio uses the Sony primary HAL with the A37 NVRAM runtime and MT6750 mixer
configuration. NVRAM initialization runs before the binderized HAL. The HAL
service has the system group required to read calibration data. Platform
remote-submix and USB audio modules are selected explicitly, since the donor's
remote-submix input reports an invalid buffer size on this userspace.
Framework AudioTrack playback is audible and AudioRecord capture returns
nonzero microphone data. No audio framework deadlock workaround is needed.

Wi-Fi/P2P and USB accessory declarations keep the corresponding framework
services available to Settings. The connected-devices dashboard also requires
UsbManager. Debugfs is mounted for kernel wake-source statistics and the
graphics memory HAL.

Backlight uses mtk/lights/lights.c with the retained kernel 0–255 sysfs interface.
The donor lights HAL, liblights-core and led_service are removed.
The white PMIC indicator is shared by notification and battery light IDs;
keyboard, button and RGB-only light controls are not advertised.

Software codec registration is rooted at vendor/etc/media_codecs.xml so OGG
ringtones can find OMX.google.vorbis.decoder. Settings ringtone playback was
confirmed with the Sony primary HAL retained.

Fuel gauging uses the Hermes Oreo loader and the matching A37 algorithm at
/vendor/lib/libfgauge.so.
The daemon starts in Android and charger modes; its logger can write kmsg as
system. The board health module corrects the kernel millivolt voltage interface
without changing the platform BatteryMonitor or reported capacity values.

A relative vendor/firmware -> etc/firmware link supports the donor connectivity
launcher while retaining the single A37 firmware directory.

Connectivity starts the Sony WMT loader/launcher against the firmware alias.
The MediaTek Wi-Fi HAL is built from mtk/wlan/wifi_hal/; its service needs privileged
interface operations on this legacy kernel. Bluetooth retains the Sony vendor
libraries, with complete H4 writes and a bounded extended-feature page query.
The Wi-Fi HAL reads regulatory channels through nl80211 rather than the
kernel's stub vendor channel list. Wificond uses the local BSS age when the
legacy kernel does not provide a boot-time timestamp. Scanning, association,
DHCP and Wi-Fi-bound Internet traffic were verified with WTA541 on 5 GHz.
Bluetooth discovery, pairing and connection were confirmed by the user;
Bluetooth audio remains untested.

The donor sensor library contains only a multi-HAL loader with no sub-HAL.
The target sensor HAL and MMC3416x calibration daemon supply the physical
sensors, with a version-aware Oreo adapter. Acceleration, light, proximity
and magnetic-field events were observed through SensorManager. Sensor list
entries alone do not prove that optional gyro/step sensors are functional.

Automatic brightness uses the physical ALS and the target lux/backlight curve;
the donor disable flag is removed. The user confirmed changes in ambient light
now adjust display brightness.

Radio uses the Hermes/Daniel HK open-source MTK Oreo adapter in mtk/ril/, with
its paired RIL/MAL binaries and the target modem firmware. CCCI initializes the
modem and starts the serial multiplexer before the HIDL daemon. Both SIM slots
share modem 1. Reading the real baseband version and SIM2 READY is established;
this alone does not validate calls, IMS or mobile data traffic.

Charger rendering uses RGBX_8888 and the configured LED-class backlight restore
after framebuffer unblank. System is already available from first-stage init.

Camera follows the Hermes HAL1 arrangement: the platform provider runs in
mediaserver through passthrough HIDL. Blobs stay in normal vendor/lib paths.
A37 factory sensor, ISP, 3A and paired DP/PQ libraries replace incompatible
Sony camera components; the Sony sensor listener remains compatible. The Hermes
MM feature bridge uses the Oreo BufferItem ABI. No private copy of platform
libui/libgui/libbinder/libutils/libc++ is included. Camera provenance records
exact file hashes. The source shims forward old graphics calls to the current
platform and route the factory JPEG encoder to libmtkjpeg's extended v62 ABI.
PQ starts from a normal init service. The retained media UID gains the system
and graphics groups for calibration and driver access. Camera platform changes
are exported under patches/. Front/rear preview and JPEG passed after cold boot
with ordinary service permissions on 2026-11-22; recording remains unverified.

The stock Camera2 application starts with the packaged media profiles and saved
a 2448x3264 JPEG. Video profiles are declared; recording has not been validated
with this camera integration.

GNSS uses the existing Sony gps.mt6755 HAL, mnld, libmnl, mtk_agpsd and wifi2agps
in their ordinary vendor paths, exposed through the platform GNSS 1.0 adapter.
The device init file creates GPS data directories and grants the daemon access
to the kernel power controls. libcurl is built from platform source. Platform
patches initialize the AGnss RIL callbacks before use and copy length-delimited
NMEA buffers into terminated HIDL strings. The China MCC overlay uses the NTP
server that responded on the test network. Satellite reception passed at the
window; an actual position fix still requires verification with better sky view.

Compressed swap is enabled through fstab and swapon_all at boot: one 512 MiB
zram device, LZ4, eight compression streams. The kernel keeps its existing
LZ4 implementation with Wingtech-derived fixes for unaligned buffer copies,
compression-stream allocation failure, and scratch buffer initialization.
An 8 MiB mixed-data round trip at a 512-byte offset passed byte-for-byte;
compressed storage was 3,204,835 bytes. Swap was restored after the test and
automatic activation was verified after reboot.

Lineage-specific resources live in overlay-lineage/, following wt88047's
layout but using the retained controller's Home/Back/Recents/volume keys (83),
volume wake mask (64), and one non-RGB PMIC light (capability mask 72).
Button backlight is absent. The lights HAL maps the white indicator to off=0,
hardware breathing=1 and steady=255; 2..255 are the same hardware brightness.
Notifications take priority over charging indication and clearing a notification
restores the battery state. The owner confirmed charging cable insertion/removal
and SMS notification breathing on the physical device.

Developer options avoid querying OEM lock services when OEM unlocking is not
supported. The Settings patch is exported under patches/packages/apps/Settings/.

The product configuration lives in device.mk: packages, HIDL adapters and
configuration copies share short purpose headings in alphabetical order.
manifest.xml records the corresponding interfaces. Active overlays share overlay/ and
Lineage resources remain in overlay-lineage/. Unused platform/ donor scaffolding
and duplicate configuration tables are removed. The kernel tree and firmware
layout are unchanged by this reorganization.

See HALS.md for the Hermes lineage-16.0 comparison, actual reuse decisions and
vendor duplicates removed. FMRadio is excluded: DSP initialization could not
be established on this board. Its application, custom library, ioctl patch,
audio routes and device permissions are removed. FM50AF remains a camera lens
actuator node and is unrelated to broadcast radio.

proprietary-files.txt, extract-files.sh and setup-makefiles.sh follow the
LineageOS wt88047 extraction workflow. The A.26 factory OTA is the stock
reference: 63 matching files have no pin, and 462 donor/local files retain
pinned SHA-1 hashes. Source-to-destination entries handle the move into vendor/.
The extractor stages all files, verifies pinned hashes, and reapplies DT_NEEDED
fixups before replacing proprietary/. A missing file or hash mismatch leaves
the existing blob checkout intact. Donor init files can be extracted but are
excluded from generated build rules. Existing provenance JSON files are retained.
