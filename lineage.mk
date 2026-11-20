$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
$(call inherit-product, device/oppo/A37/device.mk)
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

PRODUCT_DEVICE := A37
PRODUCT_NAME := lineage_A37
PRODUCT_BRAND := OPPO
PRODUCT_MODEL := A37m
PRODUCT_MANUFACTURER := OPPO
PRODUCT_RELEASE_NAME := A37
PRODUCT_RESTRICT_VENDOR_FILES := false
