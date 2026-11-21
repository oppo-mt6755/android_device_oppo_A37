# OPPO A37 — SonyCustoms Oreo base

This device tree preserves the history of SonyCustoms/device_sony_tuba (oreo,
base c348e1a66e0b). The common MTK configuration remains in platform/.
The first target is Android boot with display, touch and USB ADB. Other hardware
services are enabled only after checking the donor HAL against the target kernel.

Build with `tools/build-android.sh bootimage systemimage` from this directory
or invoke the script from the source root. The default output is
`out-sony-restart`, separate from the previous port. Android's init binary is
used directly; the donor's shell-init debug scripts and global libtuba preload
are disabled.

The kernel remains `kernel/oppo/mt6750`, configuration
`oppo6750_15127_debug_defconfig`. Its DT mounts system in first-stage init.
The real device has 16 MiB boot/recovery and a 2 GiB system partition.
Recovery uses its own fstab; the Android fstab does not remount system.

Vendor comes from SonyCustoms/vendor_sony_tuba. A37 firmware is installed only
at `/system/vendor/etc/firmware`. Sony r12 Mali requires kernel ABI 10.2,
whereas the retained OPPO driver implements ABI 8.0. The matching OPPO Mali,
GPU auxiliary library and gralloc replace those three donor components.
32-bit and 64-bit isolated EGLImage/pixel-readback probes pass with this mix.
The OPPO libstlport runtime is included because the stock GPU imports it.
The donor HWC frame structs also differ from the retained kernel; the source
composer in hwcomposer/ uses the actual kernel header. See the vendor's
firmware-provenance.json and graphics-provenance.json for exact sources.

Only one old atomic C symbol is supplied by compat/. No global preload is used.
Necessary platform changes are exported under patches/ with their reasons.
SELinux is permissive during bring-up. Android reaches the Trebuchet desktop;
physical display and touch were confirmed on 2026-11-20. USB ADB also works.
Radio, cameras, connectivity and suspend remain under bring-up.

Audio uses the Sony primary HAL with the A37 NVRAM runtime and MT6750 mixer
configuration. NVRAM initialization runs before the binderized HAL. The HAL
service has the system group required to read calibration data. Platform
remote-submix and USB audio modules are selected explicitly, since the donor's
remote-submix input reports an invalid buffer size on this userspace.
Framework AudioTrack playback is audible and AudioRecord capture returns
nonzero microphone data. No audio framework deadlock workaround is needed.

Wi-Fi/P2P feature declarations keep the framework services available to the
Settings dashboard during connectivity bring-up. This does not establish
working scanning or network association. Debugfs is mounted for kernel wake
source statistics and the graphics memory HAL.

Backlight uses lights/lights.c with the retained kernel 0–255 sysfs interface.
The donor lights HAL, liblights-core and led_service are removed. Unsupported
logical light IDs return ENOSYS; only the real LCD backlight is advertised.

Software codec registration is rooted at vendor/etc/media_codecs.xml so OGG
ringtones can find OMX.google.vorbis.decoder. Settings ringtone playback was
confirmed with the Sony primary HAL retained.

Fuel gauging uses the matching A37 fuelgauged and /system/lib/libfgauge.so.
The daemon starts in Android and charger modes; its logger can write kmsg as
system. The board health module corrects the kernel millivolt voltage interface
without changing the platform BatteryMonitor or reported capacity values.
