DEVICE_PATH := device/oppo/A37

# Audio
PRODUCT_PACKAGES += \
    audio.r_submix.default \
    audio.usb.default \
    libtinyalsa \
    libtinycompress \
    libtinyxml
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/configs/audio/a2dp_audio_policy_configuration.xml:system/vendor/etc/a2dp_audio_policy_configuration.xml \
    $(DEVICE_PATH)/configs/audio/audio_policy_configuration.xml:system/vendor/etc/audio_policy_configuration.xml \
    $(DEVICE_PATH)/rootdir/android.hardware.audio@2.0-service.rc:system/vendor/etc/init/android.hardware.audio@2.0-service.rc \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:system/vendor/etc/audio_policy_volumes.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:system/vendor/etc/default_volume_tables.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:system/vendor/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:system/vendor/etc/usb_audio_policy_configuration.xml

# Audio HIDL
PRODUCT_PACKAGES += \
    android.hardware.audio.effect@2.0-impl \
    android.hardware.audio@2.0-impl \
    android.hardware.audio@2.0-service \
    android.hardware.soundtrigger@2.0-impl

# Bluetooth
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/rootdir/android.hardware.bluetooth@1.0-service.rc:system/vendor/etc/init/android.hardware.bluetooth@1.0-service.rc \
    frameworks/native/data/etc/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
    frameworks/native/data/etc/android.hardware.bluetooth_le.xml:system/etc/permissions/android.hardware.bluetooth_le.xml

# Bluetooth HIDL
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-impl \
    android.hardware.bluetooth@1.0-service

# Boot
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/rootdir/fstab.mt6755:root/fstab.mt6755 \
    $(DEVICE_PATH)/rootdir/init.mt6755.rc:root/init.mt6755.rc \
    $(DEVICE_PATH)/rootdir/init.mt6755.usb.rc:root/init.mt6755.usb.rc \
    $(DEVICE_PATH)/rootdir/ueventd.mt6755.rc:root/ueventd.mt6755.rc

# Camera
PRODUCT_PACKAGES += \
    Snap \
    libshim_jpeg \
    libshim_perf \
    mtkui
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/configs/media_profiles.xml:system/vendor/etc/media_profiles_V1_0.xml \
    $(DEVICE_PATH)/rootdir/mediaserver.rc:system/etc/init/mediaserver.rc \
    $(DEVICE_PATH)/rootdir/pq.rc:system/vendor/etc/init/pq.rc

# Camera HIDL
PRODUCT_PACKAGES += \
    android.hardware.camera.provider@2.4-impl

# Charger
PRODUCT_PACKAGES += \
    charger_res_images

# Codecs
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/configs/media_codecs.xml:system/vendor/etc/media_codecs.xml \
    $(DEVICE_PATH)/seccomp/mediacodec.policy:system/vendor/etc/seccomp_policy/mediacodec.policy \
    $(DEVICE_PATH)/seccomp/mediaextractor.policy:system/vendor/etc/seccomp_policy/mediaextractor.policy \
    frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/vendor/etc/media_codecs_google_audio.xml \
    frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:system/vendor/etc/media_codecs_google_video.xml

# Connectivity
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/rootdir/init.mt6755.connectivity.rc:root/init.mt6755.connectivity.rc

# Dalvik
$(call inherit-product, frameworks/native/build/phone-xhdpi-2048-dalvik-heap.mk)

# Display
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xhdpi
PRODUCT_AAPT_PREBUILT_DPI := xhdpi hdpi

# DRM HIDL
PRODUCT_PACKAGES += \
    android.hardware.drm@1.0-impl \
    android.hardware.drm@1.0-service

# GPS
PRODUCT_PACKAGES += \
    libcurl
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/rootdir/gps.rc:system/vendor/etc/init/gps.rc \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml

# GPS HIDL
PRODUCT_PACKAGES += \
    android.hardware.gnss@1.0-impl \
    android.hardware.gnss@1.0-service

# Graphics
PRODUCT_PACKAGES += \
    hwcomposer.mt6755 \
    libgralloc_extra \
    libion \
    libutilscallstack

# Graphics HIDL
PRODUCT_PACKAGES += \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.composer@2.1-service \
    android.hardware.graphics.mapper@2.0-impl

# Health HIDL
PRODUCT_PACKAGES += \
    android.hardware.health@1.0-impl

# IMS
PRODUCT_PACKAGES += \
    ImsService \
    libmtk_ims_bionic \
    libmtk_ims_ifc \
    libmtk_ims_sockets \
    libshim_ims_vt
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.telephony.ims.xml:system/etc/permissions/android.hardware.telephony.ims.xml \
    $(DEVICE_PATH)/configs/privapp-permissions-ims.xml:system/etc/permissions/privapp-permissions-ims.xml \
    $(DEVICE_PATH)/mtk/ims/init.mt6755.ims.rc:root/init.mt6755.ims.rc

# Keymaster HIDL
PRODUCT_PACKAGES += \
    android.hardware.keymaster@3.0-impl \
    android.hardware.keymaster@3.0-service

# Lights
PRODUCT_PACKAGES += \
    lights.mt6755
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/rootdir/lights.rc:system/vendor/etc/init/lights.rc

# Lights HIDL
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-impl \
    android.hardware.light@2.0-service

# Manifest
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/manifest.xml:system/vendor/manifest.xml

# Memtrack HIDL
PRODUCT_PACKAGES += \
    android.hardware.memtrack@1.0-impl \
    android.hardware.memtrack@1.0-service

# Overlays
DEVICE_PACKAGE_OVERLAYS += $(DEVICE_PATH)/overlay

# Permissions
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
    frameworks/native/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml

# Radio
PRODUCT_PACKAGES += \
    librilmtk \
    mtk-ril
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/rootdir/init.mt6755.radio.rc:root/init.mt6755.radio.rc \
    frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/vendor/etc/media_codecs_google_telephony.xml \
    frameworks/native/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml

# Sensors
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/rootdir/android.hardware.sensors@1.0-service.rc:system/vendor/etc/init/android.hardware.sensors@1.0-service.rc \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml

# Sensors HIDL
PRODUCT_PACKAGES += \
    android.hardware.sensors@1.0-impl \
    android.hardware.sensors@1.0-service

# Shims
PRODUCT_PACKAGES += \
    libshim_atomic \
    libshim_binder \
    mtkc \
    mtklog

# Storage
PRODUCT_PACKAGES += \
    e2fsck \
    mke2fs
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.sdcardfs=true

# USB
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml

# USB HIDL
PRODUCT_PACKAGES += \
    android.hardware.usb@1.0-service

# Vendor
$(call inherit-product, vendor/oppo/A37/A37-vendor.mk)

# Vibrator HIDL
PRODUCT_PACKAGES += \
    android.hardware.vibrator@1.0-impl \
    android.hardware.vibrator@1.0-service

# Wi-Fi
PRODUCT_PACKAGES += \
    hostapd \
    lib_driver_cmd_mt66xx \
    libwpa_client \
    wpa_supplicant
PRODUCT_COPY_FILES += \
    $(DEVICE_PATH)/configs/wifi/p2p_supplicant_overlay.conf:system/vendor/etc/wifi/p2p_supplicant_overlay.conf \
    $(DEVICE_PATH)/configs/wifi/wpa_supplicant.conf:system/vendor/etc/wifi/wpa_supplicant.conf \
    $(DEVICE_PATH)/configs/wifi/wpa_supplicant_overlay.conf:system/vendor/etc/wifi/wpa_supplicant_overlay.conf \
    $(DEVICE_PATH)/rootdir/android.hardware.wifi@1.0-service.rc:system/vendor/etc/init/android.hardware.wifi@1.0-service.rc \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml
PRODUCT_PROPERTY_OVERRIDES += \
    ro.boot.wificountrycode=00

# Wi-Fi HIDL
PRODUCT_PACKAGES += \
    android.hardware.wifi@1.0-service
