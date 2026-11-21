# Keep one copy of target firmware while supporting the donor's fixed lookup path.
firmware_compat_link := $(TARGET_OUT_VENDOR)/firmware
$(firmware_compat_link):
	@mkdir -p $(dir $@)
	@ln -sfn etc/firmware $@
ALL_DEFAULT_INSTALLED_MODULES += $(firmware_compat_link)
