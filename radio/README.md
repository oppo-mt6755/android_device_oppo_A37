# MediaTek Oreo radio adapter

Imported from ibilux/android_device_tree_hermes lineage-15.1,
commit 9ba8991a1ad33a8d5cbfe6e02e53161754ce1315 (Daniel HK / Hermes MTK RIL).
Original licensing and copyright headers are retained. The adapter publishes
Oreo radio HIDL services. The previous private bridge is not included.

Its paired vendor RIL comes from ibilux/android_vendor_hermes lineage-15.1,
8c9d6372915e4ec4d3207e39cc4a35c0aae9125e. Those binary components originate
from Android M, despite the LineageOS 15.1 branch. See radio-provenance.json.
The target's modem firmware and NVRAM runtime are retained.

Integration changes are restricted to build paths and dependency modules,
correct function-pointer types, completing the log shim va_list lifetime,
and removing the on-client-connect modem reset. CCCI owns modem startup;
connecting the framework to HIDL must preserve startup SIM notifications.
The boot configuration supplies the omitted shared-modem count and modem
selection options, with the matching target firmware retained.
