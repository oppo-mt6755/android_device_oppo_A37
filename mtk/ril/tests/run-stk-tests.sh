#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
stk_test=$(mktemp)
trap 'rm -f "$stk_test"' EXIT
"${CXX:-c++}" -std=c++11 -Wall -Wextra -Werror -fsanitize=address,undefined \
    -fno-omit-frame-pointer stk_alpha_test.cpp -o "$stk_test"
"$stk_test"
