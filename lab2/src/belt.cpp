#include "belt.h"

void belt_init(uint8_t *ks, const uint8_t *k, std::size_t klen) {
  std::size_t i;
  switch (klen) {
  case 16:
    for (i = 0; i < 16; ++i) {
      ks[i + 0] = k[i];
      ks[i + 16] = k[i];
    }
    break;

  case 24:
    for (i = 0; i < 24; ++i) {
      ks[i] = k[i];
    }
    store32(ks + 24, load32(k + 0) ^ load32(k + 4) ^ load32(k + 8));
    store32(ks + 28, load32(k + 12) ^ load32(k + 16) ^ load32(k + 20));
    break;

  case 32:
    for (i = 0; i < 32; ++i) {
      ks[i] = k[i];
    }
    break;
  }
}

void belt_encrypt(uint8_t *out, const uint8_t *in, const uint8_t *ks) {
  uint32_t a = load32(in + 0);
  uint32_t b = load32(in + 4);
  uint32_t c = load32(in + 8);
  uint32_t d = load32(in + 12);

  uint32_t e;
  std::size_t i;
  uint32_t tmp;
  uint32_t key[8] = {0};

  for (i = 0; i < 8; ++i) {
    key[i] = load32(ks + (4 * i));
  }

  for (i = 0; i < 8; ++i) {
    b ^= G((a + key[KeyIndex[i][0]]), H, 5);
    c ^= G((d + key[KeyIndex[i][1]]), H, 21);
    a -= G((b + key[KeyIndex[i][2]]), H, 13);
    e = (G((b + c + key[KeyIndex[i][3]]), H, 21) ^ (uint32_t)(i + 1));
    b += e;
    c -= e;
    d += G((c + key[KeyIndex[i][4]]), H, 13);
    b ^= G((a + key[KeyIndex[i][5]]), H, 21);
    c ^= G((d + key[KeyIndex[i][6]]), H, 5);
    SWAP(a, b, tmp);
    SWAP(c, d, tmp);

    SWAP(b, c, tmp);
  }
  store32(out + 0, b);
  store32(out + 4, d);
  store32(out + 8, a);
  store32(out + 12, c);
}

void belt_decrypt(uint8_t *out, const uint8_t *in, const uint8_t *ks) {
  uint32_t a = load32(in + 0);
  uint32_t b = load32(in + 4);
  uint32_t c = load32(in + 8);
  uint32_t d = load32(in + 12);
  uint32_t e;
  std::size_t i;
  uint32_t tmp;
  uint32_t key[8] = {0};

  for (i = 0; i < 8; ++i) {
    key[i] = load32(ks + (4 * i));
  }

  for (i = 0; i < 8; ++i) {
    b ^= G((a + key[KeyIndex[7 - i][6]]), H, 5);
    c ^= G((d + key[KeyIndex[7 - i][5]]), H, 21);
    a -= G((b + key[KeyIndex[7 - i][4]]), H, 13);
    e = (G((b + c + key[KeyIndex[7 - i][3]]), H, 21) ^ (uint32_t)(7 - i + 1));
    b += e;
    c -= e;
    d += G((c + key[KeyIndex[7 - i][2]]), H, 13);
    b ^= G((a + key[KeyIndex[7 - i][1]]), H, 21);
    c ^= G((d + key[KeyIndex[7 - i][0]]), H, 5);
    SWAP(a, b, tmp);
    SWAP(c, d, tmp);
    SWAP(a, d, tmp);
  }
  store32(out + 0, c);
  store32(out + 4, a);
  store32(out + 8, d);
  store32(out + 12, b);
}
