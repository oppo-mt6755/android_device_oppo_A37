// Copyright (C) 2026 The LineageOS Project
// SPDX-License-Identifier: Apache-2.0
#include "../libril/stk_alpha.h"
#include <assert.h>
#include <iostream>

static void check(const char* hex, const char* expected) {
    std::string out;
    assert(mtk_stk::decode(hex, out));
    if (out != expected) std::cerr << hex << ": got " << out << ", expected " << expected << "\n";
    assert(out == expected);
}

int main() {
    // The actual SMS-success URC: +STKCTRL: 18,"",<address>.
    const char* fields[] = {"18", "", "819999F9"};
    std::string out = "previous";
    assert(mtk_stk::notification(fields, sizeof(fields), out));
    assert(out.empty());
    fields[1] = "53656E74";
    assert(mtk_stk::notification(fields, sizeof(fields), out));
    assert(out == "Sent");
    assert(!mtk_stk::notification(fields, sizeof(fields) - 1, out));
    assert(!mtk_stk::notification(nullptr, sizeof(fields), out));
    fields[0] = nullptr;
    assert(!mtk_stk::notification(fields, sizeof(fields), out));
    fields[0] = "";
    assert(!mtk_stk::notification(fields, sizeof(fields), out));
    fields[0] = "abc";
    assert(!mtk_stk::notification(fields, sizeof(fields), out));
    fields[0] = "1234";
    assert(!mtk_stk::notification(fields, sizeof(fields), out));
    fields[0] = "18";
    fields[1] = nullptr;
    assert(!mtk_stk::notification(fields, sizeof(fields), out));

    check("", "");
    check("53656e74FF", "Sent");
    check("00010210", "@£$Δ");
    check("1B141B281B291B2F1B3C1B3D1B3E1B401B65", "^{}\\[~]|€");
    check("806210529F", "成功");
    check("806210529FFFFF", "成功");
    check("806210529FFFFFff", "成功");
    check("81029C8096", "一世");
    check("82024E008096", "一世");
    check("81039C4180FF", "A一乿");
    check("82034E004180FF", "A一乿");
    check("FF", "");
    check("80ff", "");
    for (const char* bad : {"0", "GG", "800000", "80d800", "81",
                            "8102", "81029C80", "82034E008016", "1B",
                            "81029C1B80", "8300"}) {
        assert(!mtk_stk::decode(bad, out));
    }
    assert(!mtk_stk::decode(nullptr, out));
    std::string tooLong(1026, '0');
    assert(!mtk_stk::decode(tooLong.c_str(), out));
    // Exhaust malformed bytes with valid storage under ASan/UBSan.
    for (unsigned i = 0; i <= 0xffff; ++i) {
        char hex[5];
        snprintf(hex, sizeof(hex), "%04x", i);
        mtk_stk::decode(hex, out);
    }
    std::cout << "STK alpha decoding tests passed\n";
}
