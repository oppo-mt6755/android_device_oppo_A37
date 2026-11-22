// Copyright (C) 2026 The LineageOS Project
// SPDX-License-Identifier: Apache-2.0

#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>
#include <string.h>

namespace mtk_stk {
inline void utf8(uint32_t code, std::string& out) {
    if (code < 0x80) out += static_cast<char>(code);
    else if (code < 0x800) {
        out += static_cast<char>(0xc0 | (code >> 6));
        out += static_cast<char>(0x80 | (code & 0x3f));
    } else {
        out += static_cast<char>(0xe0 | (code >> 12));
        out += static_cast<char>(0x80 | ((code >> 6) & 0x3f));
        out += static_cast<char>(0x80 | (code & 0x3f));
    }
}
inline uint32_t gsm(uint8_t value, bool extended) {
    if (extended) {
        switch (value) {
        case 0x0a: return '\f'; case 0x14: return '^'; case 0x28: return '{';
        case 0x29: return '}'; case 0x2f: return '\\'; case 0x3c: return '[';
        case 0x3d: return '~'; case 0x3e: return ']'; case 0x40: return '|';
        case 0x65: return 0x20ac; default: return 0xfffd;
        }
    }
    static const uint16_t table[128] = {
        '@',0xa3,'$',0xa5,0xe8,0xe9,0xf9,0xec,0xf2,0xc7,'\n',0xd8,0xf8,'\r',0xc5,0xe5,
        0x394,'_',0x3a6,0x393,0x39b,0x3a9,0x3a0,0x3a8,0x3a3,0x398,0x39e,0xfffd,0xc6,0xe6,0xdf,0xc9,
        ' ','!','"','#',0xa4,'%','&','\'', '(',')','*','+',',','-','.','/',
        '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?',
        0xa1,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
        'P','Q','R','S','T','U','V','W','X','Y','Z',0xc4,0xd6,0xd1,0xdc,0xa7,
        0xbf,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
        'p','q','r','s','t','u','v','w','x','y','z',0xe4,0xf6,0xf1,0xfc,0xe0
    };
    return table[value & 0x7f];
}
// TS 31.102/51.011 ADN alpha encoding, including 0x80/81/82 UCS-2 forms.
inline bool decode(const char* hex, std::string& out) {
    out.clear();
    if (!hex) return false;
    const size_t length = strnlen(hex, 1025);
    if (length > 1024 || length % 2) return false;
    auto nibble = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return -1;
    };
    std::vector<uint8_t> bytes;
    bytes.reserve(length / 2);
    for (size_t i = 0; i < length; i += 2) {
        const int high = nibble(hex[i]), low = nibble(hex[i + 1]);
        if (high < 0 || low < 0) return false;
        bytes.push_back((high << 4) | low);
    }
    if (bytes.empty()) return true;
    if (bytes[0] == 0x80) {
        size_t end = bytes.size();
        if ((end - 1) % 2 && bytes.back() == 0xff) --end;
        if ((end - 1) % 2) return false;
        for (size_t i = 1; i < end; i += 2) {
            uint32_t code = (bytes[i] << 8) | bytes[i+1];
            if (code == 0xffff) break;
            if (!code || (code >= 0xd800 && code <= 0xdfff)) return false;
            utf8(code, out);
        }
        return true;
    }
    size_t start = 0, end = bytes.size();
    uint32_t base = 0;
    bool compressed = bytes[0] == 0x81 || bytes[0] == 0x82;
    if (compressed) {
        start = bytes[0] == 0x81 ? 3 : 4;
        if (bytes.size() < start || bytes[1] > bytes.size() - start) return false;
        end = start + bytes[1];
        base = bytes[0] == 0x81 ? bytes[2] << 7 : (bytes[2] << 8) | bytes[3];
    }
    bool escaped = false;
    for (size_t i = start; i < end; ++i) {
        uint8_t value = bytes[i];
        if (!compressed && value == 0xff) break;
        if (compressed && (value & 0x80)) {
            if (escaped) return false;
            uint32_t code = base + (value & 0x7f);
            if (!code || code > 0xffff || (code >= 0xd800 && code <= 0xdfff)) return false;
            utf8(code, out);
        } else {
            if (value > 0x7f) return false;
            if (value == 0x1b && !escaped) { escaped = true; continue; }
            utf8(gsm(value, escaped), out); escaped = false;
        }
    }
    if (escaped) { out.clear(); return false; }
    return true;
}
// MTK v10 reports [control type, ADN alpha hex, modified address].
// The AOSP indication expects decoded UTF-8, not the pointer array.
inline bool notification(const void* data, size_t size, std::string& out) {
    out.clear();
    if (!data || size != 3 * sizeof(char*)) return false;
    const auto* fields = static_cast<const char* const*>(data);
    if (!fields[0] || !*fields[0]) return false;
    size_t i = 0;
    for (; i < 3 && fields[0][i]; ++i)
        if (fields[0][i] < '0' || fields[0][i] > '9') return false;
    if (i == 3 && fields[0][i]) return false;
    return decode(fields[1], out);
}
} // namespace mtk_stk
