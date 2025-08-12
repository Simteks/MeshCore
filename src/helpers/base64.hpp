#pragma once

#include <cstddef>
#include <cstdint>

// Simple Base64 decoder.
// Returns number of decoded bytes written to out.
// Ignores whitespace; supports '=' padding.
static inline int decode_base64(const unsigned char* input, std::size_t in_len, uint8_t* out) {
  auto decode_char = [](unsigned char c) -> int {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    if (c == '=') return -2; // padding
    // skip whitespace and invalid characters
    return -1;
  };

  int out_len = 0;
  int val = 0;
  int valb = -8;
  int pad = 0;

  for (std::size_t i = 0; i < in_len; ++i) {
    int d = decode_char(input[i]);
    if (d == -1) {
      // ignore whitespace or invalid characters
      continue;
    }
    if (d == -2) { // '=' padding
      pad++;
      // treat as zero; advance state to finalize
      d = 0;
    }
    val = (val << 6) + d;
    valb += 6;
    if (valb >= 0) {
      uint8_t byte = (uint8_t)((val >> valb) & 0xFF);
      if (pad == 0) {
        out[out_len++] = byte;
      } else {
        // if padding has started, we should stop emitting real bytes depending on pad count
        // Standard Base64 padding rules ensure the decoder won't add extra bytes here
      }
      valb -= 8;
    }
  }

  // Adjust for padding if present (remove spurious bytes at end)
  if (pad > 0) {
    if (pad == 1) {
      // one '=' means last quantum had 3 useful bytes; decoder may have emitted 3 already
      // out_len is correct
    } else if (pad >= 2) {
      // two '=' means only 2 useful bytes in last quantum
      // out_len is correct as we didn't emit beyond data
    }
  }

  return out_len;
}
