# Hermes HAL and layout comparison

Reference: [ibilux/android_device_tree_hermes, lineage-16.0](https://github.com/ibilux/android_device_tree_hermes/tree/lineage-16.0),
commit `3a0248b9a117ad1292f393b89671e972d1e2876e`.
The target remains LineageOS 15.1 / Android 8.1 and the existing MT6750 kernel.

| Component | Decision and reason |
| --- | --- |
| HIDL adapters | Reuse the matching Android 8.1 platform implementations for audio/effects, Bluetooth, camera HAL1, GNSS, allocator/mapper/composer, health, keymaster, lights, memtrack, sensors, USB, vibrator and DRM. Grouped by purpose in device.mk. |
| RIL | Retain the already adapted Hermes/Daniel HK Oreo HIDL daemon and native RIL under mtk/ril; keep the validated IMS protocol fixes. The Pie radio interfaces do not replace them. |
| Wi-Fi | Keep the Hermes-derived MTK nl80211 implementation and target regulatory fixes, now under mtk/wlan. |
| Graphics extension | Build Hermes libgralloc_extra from mtk/graphics/gralloc_extra, with matching 32/64-bit exports and real buffer query checks; front/rear preview and JPEG capture also pass. Keep target-matched gralloc/Mali and the source composer matching the OPPO display ioctls. |
| Lights and health | Keep the target source modules under mtk/lights and mtk/health for the white breathing LED and battery voltage units. |
| Bluetooth libraries | Retain the working donor libraries. Hermes source initialization requires matching chip/NVRAM contracts; it is not justified by a directory move. |
| GPS | Hermes mtk/gps still wraps a prebuilt mtk_agpsd; retain the current tested MTK HAL/daemon set rather than substitute a different binary. Satellite reception works; a position fix remains unverified. |
| Power | Hermes powerHint and setInteractive are empty. Keep the current implementation until meaningful kernel controls are adapted; copying its placeholder would not add power management. |
| Thermal | Hermes lists platform wrappers but supplies no matching legacy thermal HAL. Do not advertise an unimplemented adapter or unscaled millidegree readings. Kernel protection remains separate. |
| LiveDisplay | Pie uses vendor.lineage.livedisplay@2.0 and /sys/devices/platform/mtk_disp_mgr.0/rgb. This target lacks that node; it cannot be directly reused. |
| Health 2.0 / configstore 1.1 / DRM 1.1 | Pie versions do not belong in the Android 8.1 product. Keep the corresponding Oreo interfaces. |
| IR and FM | IR hardware is absent. FM DSP initialization could not be established; exclude both features. |

## Removed donor duplicates

Both 32-bit and 64-bit copies of hwcomposer.mt6755, audio.usb.mt6755,
audio.r_submix.mt6755, libdrmclearkeyplugin and libmockdrmcryptoplugin are removed.
Source modules already replace these implementations. libgralloc_extra donor
copies are removed; the source implementation passes exported-symbol coverage
and 32/64-bit width, height, stride and pixel-format buffer queries.
No vendor ELF declares a direct DT_NEEDED dependency on the removed HAL/plugin
names. The audio properties select audio.usb.default and audio.r_submix.default,
and process maps confirmed those actual modules before removal.

Legacy camera.mt6755, gps.mt6755, sensors.mt6755, audio.primary.mt6755,
gralloc.mt6755 and memtrack.mt6755 are backend HALs loaded by the platform HIDL
adapters, not duplicate implementations of those adapters.

## Directory layout

- device.mk: native/HIDL packages, permissions and configurations, grouped by purpose.
- lineage.mk: product identity and base inheritance.
- manifest.xml: Oreo interface declarations.
- mtk/: graphics, health, lights, ril, ims and wlan implementations.
- libshims/: native misc, graphics, JPEG and IMS compatibility symbols.
- configs/, rootdir/, overlay/, overlay-lineage/: target configuration.
- patches/: platform changes required by the retained kernel and legacy HALs.

The former platform/ subtree, disabled libtuba module and FM adaptation are
removed. The target defconfig disables FM and connectivity initialization skips
its optional FM branch; Wi-Fi, Bluetooth and GPS stay enabled. Active overlay resources are merged without duplicate resource names.
