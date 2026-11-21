#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
ims_test=$(mktemp)
trap 'rm -f "$ims_test"' EXIT
cc -std=gnu11 -Wall -Wextra -Werror -Iinclude ../ifc_ccmni.c prefix_test.c -o "$ims_test"
"$ims_test"
