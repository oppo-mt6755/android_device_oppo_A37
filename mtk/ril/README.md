# MediaTek Oreo radio adapter

Imported from ibilux/android_device_tree_hermes lineage-15.1,
commit 9ba8991a1ad33a8d5cbfe6e02e53161754ce1315 (Daniel HK / Hermes MTK RIL).
Original licensing and copyright headers are retained. The adapter publishes
Oreo radio HIDL services. The previous private bridge is not included.

Its paired vendor RIL comes from ibilux/android_vendor_hermes lineage-15.1,
8c9d6372915e4ec4d3207e39cc4a35c0aae9125e. Those binary components originate
from Android M, despite the LineageOS 15.1 branch. See radio-provenance.json.
The target's modem firmware and NVRAM runtime are retained.

Initial integration changes covered build paths and dependency modules,
correct function-pointer types, completing the log shim va_list lifetime,
and removing the on-client-connect modem reset. CCCI owns modem startup;
connecting the framework to HIDL must preserve startup SIM notifications.
The boot configuration supplies the omitted shared-modem count and modem
selection options, with the matching target firmware retained.

IMS adds the native librilmtk IMS and MAL socket codecs alongside HIDL. Native
requests and their completions remain in the vendor library's own queues;
framework requests stay in the Hermes HIDL queue. The shared RequestInfo prefix
is checked at compile time. See ../ims/README.md for service sources, vendor
provenance, SIM selection, and the unsupported video-call media operations.

The native codec requires a connected legacy radio notification subscriber before
it fans unsolicited bearer updates out to MAL. The HIDL adapter holds read-only
subscriptions to rild/rild2 so that existing native notification handling remains
active. They send no radio requests; the framework still uses HIDL.

The modem RIL does not implement standard IMS_SEND_SMS. GSM IMS submissions
are converted to its existing SEND_SMS payload at dispatch, retaining the
framework IMS request token and response method. This lets the vendor choose
the SMS transport and preserves its message reference and error result.
CDMA submissions retain their existing dispatch. Queued GSM IMS messages use
the native structure size, including alignment padding.

MTK v10 STK call-control notifications contain three string pointers: control
type, ADN-encoded alpha hex, and modified address. The HIDL adapter decodes the
alpha field (GSM default/extension alphabet and UCS-2 0x80/0x81/0x82 forms).
An empty alpha requests no user message; pointer bytes must never be forwarded
as UTF-8. This fixes garbled SIM toolkit toasts after successful SMS submission.

Run `mtk/ril/tests/run-stk-tests.sh` from the device directory for the host
ASan/UBSan decoding regression tests.
