#include <cassert>
#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>

#include "block.h"
#include "const.h"

uint32_t feistelBox[8][64] = {{0}};
bool feistelBoxInited = false;

void cryptBlock(std::vector<uint64_t> &subkeys, std::vector<uint8_t> &dst,
                std::vector<uint8_t> src, bool decrypt) {
  uint64_t b = 0;
  assert(src.size() == 8);
  for (int i = 0; i < 8; i++) {
    b += (static_cast<uint64_t>(src[i]) << 8 * (7 - i));
  }

  b = permuteInitialBlock(b);

  uint32_t left = static_cast<uint32_t>(b >> 32);
  uint32_t right = static_cast<uint32_t>(b);

  left = (left << 1) | (left >> 31);
  right = (right << 1) | (right >> 31);

  if (decrypt) {
    for (uint8_t i = 0; i < 8; i++) {
      auto pair =
          feistel(left, right, subkeys[15 - 2 * i], subkeys[15 - (2 * i + 1)]);
      left = pair.first;
      right = pair.second;
    }
  } else {
    for (uint8_t i = 0; i < 8; i++) {
      auto pair = feistel(left, right, subkeys[2 * i], subkeys[2 * i + 1]);
      left = pair.first;
      right = pair.second;
    }
  }

  left = (left << 31) | (left >> 1);
  right = (right << 31) | (right >> 1);

  // switch left & right and perform final permutation
  uint64_t preOutput =
      (static_cast<uint64_t>(right) << 32) | static_cast<uint64_t>(left);

  uint64_t out = permuteFinalBlock(preOutput);
  assert(dst.size() == 8);
  for (int i = 0; i < 8; i++) {
    dst[i] = (out >> 8 * (7 - i));
  }
}

void encryptBlock(std::vector<uint64_t> &subkeys, std::vector<uint8_t> &dst,
                  const std::vector<uint8_t> &src) {
  cryptBlock(subkeys, dst, src, false);
}

void decryptBlock(std::vector<uint64_t> &subkeys, std::vector<uint8_t> &dst,
                  const std::vector<uint8_t> &src) {
  cryptBlock(subkeys, dst, src, true);
}

std::pair<uint32_t, uint32_t> feistel(uint32_t l, uint32_t r, uint64_t k0,
                                      uint64_t k1) {
  uint32_t t = 0;

  t = r ^ static_cast<uint32_t>(k0 >> 32);
  l ^= feistelBox[7][t & 0x3f] ^ feistelBox[5][(t >> 8) & 0x3f] ^
       feistelBox[3][(t >> 16) & 0x3f] ^ feistelBox[1][(t >> 24) & 0x3f];

  t = ((r << 28) | (r >> 4)) ^ static_cast<uint32_t>(k0);
  l ^= feistelBox[6][(t)&0x3f] ^ feistelBox[4][(t >> 8) & 0x3f] ^
       feistelBox[2][(t >> 16) & 0x3f] ^ feistelBox[0][(t >> 24) & 0x3f];

  t = l ^ static_cast<uint32_t>(k1 >> 32);
  r ^= feistelBox[7][t & 0x3f] ^ feistelBox[5][(t >> 8) & 0x3f] ^
       feistelBox[3][(t >> 16) & 0x3f] ^ feistelBox[1][(t >> 24) & 0x3f];

  t = ((l << 28) | (l >> 4)) ^ static_cast<uint32_t>(k1);
  r ^= feistelBox[6][(t)&0x3f] ^ feistelBox[4][(t >> 8) & 0x3f] ^
       feistelBox[2][(t >> 16) & 0x3f] ^ feistelBox[0][(t >> 24) & 0x3f];

  return std::make_pair(l, r);
}

uint64_t permuteBlock(uint64_t src, const std::vector<uint8_t> &permutation) {
  uint64_t block = 0;
  std::size_t len = permutation.size();
  for (uint8_t position = 0; position < len; position++) {
    uint8_t n = permutation[position];
    uint64_t bit = (src >> n) & 1;
    block |= (bit << static_cast<uint64_t>((len - 1) - position));
  }
  return block;
}

void initFeistelBox() {
  for (uint8_t s = 0; s < 8; s++) {
    for (uint8_t i = 0; i < 4; i++) {
      for (uint8_t j = 0; j < 16; j++) {
        uint64_t f = static_cast<uint64_t>(sBoxes[s][i][j])
                     << (4 * (7 - static_cast<unsigned long>(s)));
        f = permuteBlock(f, permutationFunction);

        // Row is determined by the 1st and 6th bit.
        // Column is the middle four bits.
        uint8_t row = static_cast<uint8_t>(((i & 2) << 4) | (i & 1));
        uint8_t col = static_cast<uint8_t>(j << 1);
        uint8_t t = row | col;

        // The rotation was performed in the feistel rounds, being factored out
        // and now mixed into the feistelBox.
        f = (f << 1) | (f >> 31);

        feistelBox[s][t] = static_cast<uint32_t>(f);
      }
    }
  }
}

uint64_t permuteInitialBlock(uint64_t block) {
  // block = b7 b6 b5 b4 b3 b2 b1 b0 (8 bytes)
  uint64_t b1 = block >> 48;
  uint64_t b2 = block << 48;
  block ^= b1 ^ b2 ^ b1 << 48 ^ b2 >> 48;

  // block = b1 b0 b5 b4 b3 b2 b7 b6
  b1 = (block >> 32 & 0xff00ff);
  b2 = (block & 0xff00ff00);
  block ^=
      (b1 << 32) ^ b2 ^ (b1 << 8) ^ (b2 << 24); // exchange b0 b4 with b3 b7

  // block is now b1 b3 b5 b7 b0 b2 b4 b7, the permutation:
  //                  ...  8
  //                  ... 24
  //                  ... 40
  //                  ... 56
  //  7  6  5  4  3  2  1  0
  // 23 22 21 20 19 18 17 16
  //                  ... 32
  //                  ... 48

  // exchange 4,5,6,7 with 32,33,34,35 etc.
  b1 = block & 0x0f0f00000f0f0000;
  b2 = block & 0x0000f0f00000f0f0;
  block ^= b1 ^ b2 ^ (b1 >> 12) ^ (b2 << 12);

  // block is the permutation:
  //
  //   [+8]         [+40]
  //
  //  7  6  5  4
  // 23 22 21 20
  //  3  2  1  0
  // 19 18 17 16    [+32]

  // exchange 0,1,4,5 with 18,19,22,23
  b1 = block & 0x3300330033003300;
  b2 = block & 0x00cc00cc00cc00cc;
  block ^= b1 ^ b2 ^ (b1 >> 6) ^ (b2 << 6);

  // block is the permutation:
  // 15 14
  // 13 12
  // 11 10
  //  9  8
  //  7  6
  //  5  4
  //  3  2
  //  1  0 [+16] [+32] [+64]

  // exchange 0,2,4,6 with 9,11,13,15:
  b1 = block & 0xaaaaaaaa55555555;
  block ^= b1 ^ (b1 >> 33) ^ (b1 << 33);

  // block is the permutation:
  // 6 14 22 30 38 46 54 62
  // 4 12 20 28 36 44 52 60
  // 2 10 18 26 34 42 50 58
  // 0  8 16 24 32 40 48 56
  // 7 15 23 31 39 47 55 63
  // 5 13 21 29 37 45 53 61
  // 3 11 19 27 35 43 51 59
  // 1  9 17 25 33 41 49 57
  return block;
}

// permuteInitialBlock is equivalent to the permutation defined
// by finalPermutation.
uint64_t permuteFinalBlock(uint64_t block) {
  // Perform the same bit exchanges as permuteInitialBlock
  // but in reverse order.
  uint64_t b1 = block & 0xaaaaaaaa55555555;
  block ^= b1 ^ (b1 >> 33) ^ (b1 << 33);

  b1 = block & 0x3300330033003300;
  uint64_t b2 = block & 0x00cc00cc00cc00cc;
  block ^= b1 ^ b2 ^ (b1 >> 6) ^ (b2 << 6);

  b1 = block & 0x0f0f00000f0f0000;
  b2 = block & 0x0000f0f00000f0f0;
  block ^= b1 ^ b2 ^ (b1 >> 12) ^ (b2 << 12);

  b1 = (block >> 32) & 0xff00ff;
  b2 = (block & 0xff00ff00);
  block ^= (b1 << 32) ^ b2 ^ (b1 << 8) ^ (b2 << 24);

  b1 = block >> 48;
  b2 = block << 48;
  block ^= b1 ^ b2 ^ (b1 << 48) ^ (b2 >> 48);
  return block;
}

std::vector<uint32_t> ksRotate(uint32_t in) {
  auto out = std::vector<uint32_t>(16);
  uint32_t last = in;
  for (uint8_t i = 0; i < 16; i++) {
    // 28-bit circular left shift
    uint32_t left = (last << (4 + ksRotations[i])) >> 4;
    uint32_t right = (last << 4) >> (32 - ksRotations[i]);
    out[i] = left | right;
    last = out[i];
  }
  return out;
}

// Expand 48-bit input to 64-bit, with each 6-bit block padded by extra two bits
// at the top. By doing so, we can have the input blocks (four bits each), and
// the key blocks (six bits each) well-aligned without extra shifts/rotations
// for alignments.
uint64_t unpack(uint64_t x) {
  return ((x >> (6 * 1)) & 0xff) << (8 * 0) |
         ((x >> (6 * 3)) & 0xff) << (8 * 1) |
         ((x >> (6 * 5)) & 0xff) << (8 * 2) |
         ((x >> (6 * 7)) & 0xff) << (8 * 3) |
         ((x >> (6 * 0)) & 0xff) << (8 * 4) |
         ((x >> (6 * 2)) & 0xff) << (8 * 5) |
         ((x >> (6 * 4)) & 0xff) << (8 * 6) |
         ((x >> (6 * 6)) & 0xff) << (8 * 7);
}
