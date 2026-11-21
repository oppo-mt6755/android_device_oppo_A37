# MediaTek Wi-Fi HAL

Imported from coolpadnote3litedevelopment/android_device_coolpad_mt6735-common
(Oreo-compatible MTK HAL) to provide real nl80211/vendor command handling.
The platform fallback HAL returns NOT_SUPPORTED at initialization.
Source license headers and attribution are preserved. This uses the retained
kernel driver and the single A37 firmware directory via the compatibility link.

Reference commit: a600edc128f66e64c8cf5d3302a519676a175129.

The legacy vendor channel-list command returns test frequencies in this
kernel. The HAL instead reads enabled regulatory channels using
NL80211_CMD_GET_WIPHY and filters the requested 2.4/5 GHz and DFS bands.
