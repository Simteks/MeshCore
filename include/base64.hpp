// Minimal Base64 decoder for MeshCore
// Provides: int decode_base64(const unsigned char* src, int len, unsigned char* out)
// Returns number of bytes written to out. Ignores whitespace. Stops at '=' padding.

#pragma once

#include <stdint.h>
#include <stddef.h>

static inline int decode_base64(const unsigned char* src, int len, unsigned char* out) {
  static const int8_t T[256] = {
    /* 0x00-0x0F */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 0x10-0x1F */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 0x20-0x2F */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 62,-1,-1,-1, 63,
    /* 0x30-0x3F */ 52,53,54,55,56,57,58,59,60,61,-1,-1,-1, -2,-1,-1,
    /* 0x40-0x4F */ -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
    /* 0x50-0x5F */ 15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
    /* 0x60-0x6F */ -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
    /* 0x70-0x7F */ 41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
    /* 0x80-0x8F */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 0x90-0x9F */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 0xA0-0xAF */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 0xB0-0xBF */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 0xC0-0xCF */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 0xD0-0xDF */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 0xE0-0xEF */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    /* 0xF0-0xFF */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
  };

  int out_len = 0;
  uint32_t buf = 0;
  int valb = -8;  // next write position

  for (int i = 0; i < len && src[i]; ++i) {
    unsigned char c = src[i];
    if (c == '=') break;               // padding
    if (c == ' ' || c == '\n' || c == '\r' || c == '\t') continue; // skip whitespace
    int8_t v = T[c];
    if (v < 0) continue;               // ignore non-base64 chars
    buf = (buf << 6) | (uint32_t)v;
    valb += 6;
    if (valb >= 0) {
      out[out_len++] = (unsigned char)((buf >> valb) & 0xFF);
      valb -= 8;
    }
  }

  return out_len;
}
