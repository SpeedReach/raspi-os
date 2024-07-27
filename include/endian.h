#ifndef ENDIAN_H
#define ENDIAN_H

#include<stdint.h>

/// Reverses the byte order in a `uint32_t`.
static inline uint32_t rev_u32(const uint32_t x) {
  uint32_t result;
  __asm__("rev %w0, %w1" : "=r"(result) : "r"(x));
  return result;
}


#endif