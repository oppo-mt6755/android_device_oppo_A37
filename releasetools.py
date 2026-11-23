# Copyright (C) 2026 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0


def FullOTA_InstallBegin(info):
    # Older TWRP kernels expose the same partition names one directory higher.
    info.script.AppendExtra(
        'run_program("/sbin/sh", "-c", '
        '"test -e /dev/block/platform/mtk-msdc.0/11230000.msdc0/by-name/boot || '
        '{ test -e /dev/block/platform/mtk-msdc.0/by-name/boot && '
        'mkdir -p /dev/block/platform/mtk-msdc.0/11230000.msdc0 && '
        'ln -s ../by-name /dev/block/platform/mtk-msdc.0/11230000.msdc0/by-name; }");')


def IncrementalOTA_InstallBegin(info):
    FullOTA_InstallBegin(info)
