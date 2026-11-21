# IMS integration

This uses the Coolpad Android M MAL and ImsService components adapted for the
Android 8.1 framework, together with the paired Hermes native librilmtk socket
codec. The three factory IMS workers are retained because the target modem
requires IMC structure version 0x50; the Coolpad 0x4f broker rejects it.
Repository revisions are recorded in provenance.json and the vendor tree's
ims-provenance.json. Original source notices are retained.

The Hermes HIDL adapter remains the main radio interface. Its RequestInfo prefix
matches the native IMS request ABI, enforced for both pointer widths at compile
time. The radio daemon publishes the legacy IMS socket environment alias required by
the native library. IMS PDNs select ccmni4; normal data uses ccmni0 through ccmni3.

The framework compatibility code reads persist.radio.simswitch to select the
modem's active SIM slot. Carrier configuration enables VoLTE for MCC 460/MNC 01.

Run `bash device/oppo/A37/ims/build-service.sh` at the Android source root to
regenerate service/ImsService.apk after editing Java or smali sources. The Android
build signs the resulting privileged package with the platform certificate.

The VT compatibility library permits the native VT service to start for IMS
initialization. It reports unavailable video media operations as unsupported;
video calling is not implemented by this adaptation.

The MAL daemon and the RIL client use paired libmdfx and libmal libraries
for compatible event messages. The Hermes mtk-ril and librilmtk remain the radio
implementation. Vendor module discovery paths are recorded as same-length binary
relocations in ims-provenance.json; firmware is unchanged.

The MAL network shim resolves its historical net.ipv6.ccmni*.prefix lookup from
an actual configured global /64 address. Link-local and unsupported prefix
lengths do not produce a fabricated result. Other property lookups retain the
normal system property behavior.

Host regression coverage for property fallback and IPv6 prefix filtering:
`bash device/oppo/A37/ims/tests/run-prefix-tests.sh`.
