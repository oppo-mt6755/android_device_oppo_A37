# mt6735-volte-fix

VoLTE for MT6735 phones running LineageOS 15.1 with the MediaTek
Android 6.0 IMS blobs. The `cm-13.0` and `cm-14.1` branches have the
CyanogenMod 13 and 14.1 versions. Tested on the Coolpad Note 3 Lite (CP8298_I00)
on Jio: IMS registration, incoming and outgoing calls with audio, and
receiving SMS over IMS.

This repo holds the stock `ImsService` from Coolpad's
6.0.007.P0.160801.8298_I00 build, deodexed to smali. Each fix is its own
commit on top of the stock import, so `git log -p` shows exactly what
was changed and why:

- Adapt to the cm-13 framework (MediaTek-only framework members are
  routed through `ImsCompat`)
- Enable IMS on the modem from `turnOnIms`
- Allow incoming calls from the call indication
- Enable IMS again when the radio comes back
- Mark the caller number as presentable
- Keep the dialed number on outgoing calls
- Implement the Nougat `IImsService` methods
- Mark the caller number as allowed on every call profile
- Send the call profile when an outgoing call starts
- Fall back to cause 0 for unknown IMS PDN fail causes
- Only hand incoming calls to the active SIM
- Pass `addRegistrationListener` on to `setRegistrationListener`; Oreo
  registers its listener there and hands `open()` a stub

`build.sh` rebuilds `ImsService.apk` from this tree. It has to be signed
with the ROM's platform key.

The device tree pins it by sha1 in `proprietary-files.txt`, so update the
pin when a new build goes into the vendor repo.

## Device, common and vendor changes

VoLTE also needs the native IMS stack, the VT service, init, sepolicy,
RIL and overlay changes in the device trees, on their `cm-14.1` branches:

- [android_device_coolpad_mt6735-common](https://github.com/coolpadnote3litedevelopment/android_device_coolpad_mt6735-common/tree/cm-14.1)
- [android_device_coolpad_CP8298_I00](https://github.com/coolpadnote3litedevelopment/android_device_coolpad_CP8298_I00/tree/cm-14.1)
- [proprietary_vendor_coolpad](https://github.com/coolpadnote3litedevelopment/proprietary_vendor_coolpad/tree/cm-14.1)

The VT service and its libraries come from the Nokia 3 7.1.1 build
(NE1 00WW_2_15H); the rest of the IMS stack is Coolpad's.

## What each piece is for

- The modem only starts IMS registration once `vtservice` is running.
- The IMS PDN has to be on ccmni4: MAL and ImsService reject any other
  interface, and MAL only supplies the P-CSCF discovery flags for it.
- The framework needs `mobile_ims` in `networkAttributes` to bring up the ims APN.
- init has 31 environment slots, so mtkrild loses its sockets
  unless the unused SIM 3 and 4 sockets are dropped.

## Android 10

The compatibility source uses Android 10 IMS parcelable names and adapts precise
data-state broadcasts to the platform APN bitmask and integer failure codes. The
single native IMS instance follows the main-capability SIM; other slot adapters
must not disable or replace its registration listener.

Run `mtk/ims/build-service.sh` from the device tree after building the platform
framework. Set `SMALI_DIR` to a directory containing `smali.jar` and
`baksmali.jar` (default: `tools/a37-host/smali` in the source root). Rebuild the
`ImsService` module to sign the resulting APK with this ROM's platform key.

The VT daemon is needed for IMS control. Video-call audio/snapshot paths remain
unsupported; the scoped shims reject these old media layouts. Camera recording
uses the normal camera/OMX pipeline and is independent of video telephony.

Android 10 registers separate MMTEL capability and registration callbacks. The
listener fanout retains both, reports LTE registration technology and supplies
six feature slots (voice/video/UT on LTE and Wi-Fi). Feature configuration
requests always acknowledge completion so framework callers do not wait for
timeouts. Unsupported UT entries return an explicit error.

Call sessions adapt the Android 10 listener AIDL to the vendor's original callback
interface. This includes the setup callbacks renamed to initiated/initiatedFailed
and removal of the redundant session argument in the newer interface.

Configuration objects retain the requesting phone ID. The empty SIM slot's
Android 10 feature updates are acknowledged without changing the single native
stack's global VoLTE/WFC properties.

Voice feature updates also send the native VoLTE and IMS voice requests; changing
the cached property alone does not update a modem that initialized with voice
disabled. Registration callbacks survive the end of a framework adapter session;
dead remote callbacks are removed by `RemoteCallbackList`.

Service publication also notifies the phone package again at boot completion.
The initial notification can precede the framework's legacy receiver/feature
connection, leaving it without an IMS phone until the next lifecycle update.
