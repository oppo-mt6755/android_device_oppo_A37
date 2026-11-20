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
The donor HWC frame structs also differ from the retained kernel; the source
composer in hwcomposer/ uses the actual kernel header. See the vendor's
firmware-provenance.json and graphics-provenance.json for exact sources.

Only one old atomic C symbol is supplied by compat/. No global preload is used.
Necessary platform changes are exported under patches/ with their reasons.
SELinux is permissive during bring-up. A successful build alone does not prove
that Android boots or that radio, cameras, audio and suspend work.
