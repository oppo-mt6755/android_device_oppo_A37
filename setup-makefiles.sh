#!/bin/bash
# Copyright (C) 2026 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
set -e

DEVICE=A37
VENDOR=oppo
INITIAL_COPYRIGHT_YEAR=2026
MY_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
LINEAGE_ROOT=$(cd "$MY_DIR/../../.." && pwd)
HELPER="$LINEAGE_ROOT/vendor/lineage/build/tools/extract_utils.sh"
if [ ! -f "$HELPER" ]; then
    echo "Unable to find helper script at $HELPER" >&2
    exit 1
fi
. "$HELPER"
setup_vendor "$DEVICE" "$VENDOR" "$LINEAGE_ROOT"
BOARDMK="$TMPDIR/BoardConfigVendor.mk"
write_headers

# Donor init files are retained for extraction; device init files are installed.
awk '!/(^|:)vendor\/etc\/init\//' "$MY_DIR/proprietary-files.txt" > "$TMPDIR/build-files.txt"
write_makefiles "$TMPDIR/build-files.txt" true
write_footers
