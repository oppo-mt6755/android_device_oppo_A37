#!/bin/bash
# Copyright (C) 2026 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
set -e

DEVICE=A37
VENDOR=oppo
MY_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
LINEAGE_ROOT=$(cd "$MY_DIR/../../.." && pwd)
HELPER="$LINEAGE_ROOT/vendor/lineage/build/tools/extract_utils.sh"
if [ ! -f "$HELPER" ]; then
    echo "Unable to find helper script at $HELPER" >&2
    exit 1
fi
. "$HELPER"
CLEAN_VENDOR=true
SECTION=
SRC=adb
while [ "$#" -gt 0 ]; do
    case "$1" in
        -n|--no-cleanup) CLEAN_VENDOR=false ;;
        -s|--section) shift; SECTION=${1:?Missing section}; CLEAN_VENDOR=false ;;
        -h|--help) echo "Usage: $0 [-n] [-s section] [adb|system-dump]"; exit 0 ;;
        *) SRC=$1 ;;
    esac
    shift
done
if [ "$SRC" != adb ]; then
    SRC=$(cd "$SRC" && pwd)
fi

# Extract into a staging directory so missing blobs cannot destroy the checkout.
VENDOR_DIR="$LINEAGE_ROOT/vendor/$VENDOR/$DEVICE"
mkdir -p "$LINEAGE_ROOT/artifacts"
EXTRACT_DIR=$(mktemp -d "$LINEAGE_ROOT/artifacts/.extract-$DEVICE-XXXXXX")
extract_cleanup() {
    python3 - "$EXTRACT_DIR" <<'PY'
from pathlib import Path
import shutil, sys
path = Path(sys.argv[1])
if path.exists():
    shutil.rmtree(path)
PY
    cleanup
}
trap extract_cleanup EXIT
cp -a "$VENDOR_DIR/proprietary" "$EXTRACT_DIR/"
cp "$VENDOR_DIR/fixup-shims.py" "$VENDOR_DIR/shim-dependencies.json" "$EXTRACT_DIR/"
setup_vendor "$DEVICE" "$VENDOR" "$LINEAGE_ROOT" false "$CLEAN_VENDOR"
OUTDIR=${EXTRACT_DIR#"$LINEAGE_ROOT/"}
extract "$MY_DIR/proprietary-files.txt" "$SRC" "$SECTION"
python3 "$EXTRACT_DIR/fixup-shims.py" --apply
python3 - "$MY_DIR/proprietary-files.txt" "$EXTRACT_DIR/proprietary" <<'PY'
from pathlib import Path
import hashlib, sys
root = Path(sys.argv[2])
for line in Path(sys.argv[1]).read_text().splitlines():
    if not line or line.startswith('#'):
        continue
    spec, _, pinned = line.lstrip('-').partition('|')
    target = spec.split(':')[-1]
    path = root / target
    if not path.is_file():
        raise SystemExit('Missing extracted blob: ' + target)
    if pinned and hashlib.sha1(path.read_bytes()).hexdigest() != pinned:
        raise SystemExit('Pinned blob changed: ' + target)
PY
"$MY_DIR/setup-makefiles.sh"
mv "$VENDOR_DIR/proprietary" "$EXTRACT_DIR/previous"
mv "$EXTRACT_DIR/proprietary" "$VENDOR_DIR/proprietary"
