#include "utility.h"

uint32_t load32(const void *in) {
  const uint8_t *p = (const uint8_t *)in;
  return ((uint32_t)p[0] << 0) | ((uint32_t)p[1] << 8) |
         ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

void store32(void *out, const uint32_t v) {
  uint8_t *p = (uint8_t *)out;
  p[0] = (uint8_t)(v >> 0);
  p[1] = (uint8_t)(v >> 8);
  p[2] = (uint8_t)(v >> 16);
  p[3] = (uint8_t)(v >> 24);
}

void store128(void *out, const uint8_t *ins) {
  uint8_t *p = (uint8_t *)out;
  for (std::size_t i = 0; i < 16; ++i) {
    p[i] = ins[i];
  }
}

void square_plus(void *out, const uint8_t *ins) {
  uint8_t *p = (uint8_t *)out;
  for (std::size_t i = 15;; i = (i + 15) % 16) {
    if (p[i] == (uint8_t)((1 << 8) - 1))
      p[i] = 0;
    else {
      ++p[i];
      break;
    }
  }
}
