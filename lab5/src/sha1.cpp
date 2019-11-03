#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "sha1.hpp"

// number of 32bit integers per SHA1 block
static const size_t BLOCK_INTS = 16;
static const size_t BLOCK_BYTES = BLOCK_INTS * 4;

static void reset(u32 digest[], std::string &buffer, u64 &transforms) {
  // SHA1 initialization constants
  digest[0] = 0x67452301;
  digest[1] = 0xefcdab89;
  digest[2] = 0x98badcfe;
  digest[3] = 0x10325476;
  digest[4] = 0xc3d2e1f0;

  // Counters
  buffer = "";
  transforms = 0;
}

static u32 rol(const u32 value, const size_t bits) {
  return (value << bits) | (value >> (32 - bits));
}

static u32 blk(const u32 block[BLOCK_INTS], const size_t i) {
  return rol(block[(i + 13) & 15] ^ block[(i + 8) & 15] ^ block[(i + 2) & 15] ^
                 block[i],
             1);
}

// (R0/R1), R2, R3, R4 are the different operations used in SHA1
static void R0(const u32 block[BLOCK_INTS], const u32 v, u32 &w, const u32 x,
               const u32 y, u32 &z, const size_t i) {
  z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + rol(v, 5);
  w = rol(w, 30);
}

static void R1(u32 block[BLOCK_INTS], const u32 v, u32 &w, const u32 x,
               const u32 y, u32 &z, const size_t i) {
  block[i] = blk(block, i);
  z += ((w & (x ^ y)) ^ y) + block[i] + 0x5a827999 + rol(v, 5);
  w = rol(w, 30);
}

static void R2(u32 block[BLOCK_INTS], const u32 v, u32 &w, const u32 x,
               const u32 y, u32 &z, const size_t i) {
  block[i] = blk(block, i);
  z += (w ^ x ^ y) + block[i] + 0x6ed9eba1 + rol(v, 5);
  w = rol(w, 30);
}

static void R3(u32 block[BLOCK_INTS], const u32 v, u32 &w, const u32 x,
               const u32 y, u32 &z, const size_t i) {
  block[i] = blk(block, i);
  z += (((w | x) & y) | (w & x)) + block[i] + 0x8f1bbcdc + rol(v, 5);
  w = rol(w, 30);
}

static void R4(u32 block[BLOCK_INTS], const u32 v, u32 &w, const u32 x,
               const u32 y, u32 &z, const size_t i) {
  block[i] = blk(block, i);
  z += (w ^ x ^ y) + block[i] + 0xca62c1d6 + rol(v, 5);
  w = rol(w, 30);
}

static void transform(u32 digest[], u32 block[BLOCK_INTS], u64 &transforms) {
  /* Copy digest[] to working vars */
  u32 a = digest[0];
  u32 b = digest[1];
  u32 c = digest[2];
  u32 d = digest[3];
  u32 e = digest[4];

  u32 *b1 = &a, *b2 = &b, *b3 = &c, *b4 = &d, *b5 = &e, *temp;

  // 4 rounds of 20 operations each
  for (u8 i = 0; i < 80; ++i) {
    if (i <= 15)
      R0(block, *b1, *b2, *b3, *b4, *b5, i % 16);
    else if (i < 20)
      R1(block, *b1, *b2, *b3, *b4, *b5, i % 16);
    else if (i < 40)
      R2(block, *b1, *b2, *b3, *b4, *b5, i % 16);
    else if (i < 60)
      R3(block, *b1, *b2, *b3, *b4, *b5, i % 16);
    else
      R4(block, *b1, *b2, *b3, *b4, *b5, i % 16);

    temp = b5;
    b5 = b4;
    b4 = b3;
    b3 = b2;
    b2 = b1;
    b1 = temp;
  }

  // Add the working vars back into digest[]
  digest[0] += a;
  digest[1] += b;
  digest[2] += c;
  digest[3] += d;
  digest[4] += e;

  // Count the number of transformations
  transforms++;
}

static void buffer_to_block(const std::string &buffer, u32 block[BLOCK_INTS]) {
  /* Convert the std::string (byte buffer) to a u32 array (MSB) */
  for (size_t i = 0; i < BLOCK_INTS; i++) {
    block[i] = (buffer[4 * i + 3] & 0xff) | (buffer[4 * i + 2] & 0xff) << 8 |
               (buffer[4 * i + 1] & 0xff) << 16 |
               (buffer[4 * i + 0] & 0xff) << 24;
  }
}

SHA1::SHA1() { reset(digest, buffer, transforms); }

std::vector<u32> SHA1::eval(const std::string &s) {
  std::istringstream is(s);
  return eval(is);
}

std::vector<u32> SHA1::eval_file(const std::string &filename) {
  std::ifstream is(filename.c_str(), std::ios::binary);
  return eval(is);
}

// Calculate
std::vector<u32> SHA1::eval(std::istream &is) {
  // Update
  while (true) {
    char sbuf[BLOCK_BYTES];
    is.read(sbuf, BLOCK_BYTES - buffer.size());
    buffer.append(sbuf, (std::size_t)is.gcount());
    if (buffer.size() != BLOCK_BYTES) {
      break;
    }
    u32 block[BLOCK_INTS];
    buffer_to_block(buffer, block);
    transform(digest, block, transforms);
    buffer.clear();
  }

  // Total number of hashed bits
  u64 total_bits = (transforms * BLOCK_BYTES + buffer.size()) * 8;

  // Padding
  buffer += (char)0x80;
  size_t orig_size = buffer.size();
  while (buffer.size() < BLOCK_BYTES) {
    buffer += (char)0x00;
  }

  u32 block[BLOCK_INTS];
  buffer_to_block(buffer, block);

  if (orig_size > BLOCK_BYTES - 8) {
    transform(digest, block, transforms);
    for (size_t i = 0; i < BLOCK_INTS - 2; i++) {
      block[i] = 0;
    }
  }

  // Append total_bits, split this u64 into two u32
  block[BLOCK_INTS - 1] = (u32)total_bits;
  block[BLOCK_INTS - 2] = (u32)(total_bits >> 32);
  transform(digest, block, transforms);

  std::vector<u32> result(std::begin(digest), std::end(digest));

  // Reset for next run
  reset(digest, buffer, transforms);

  return result;
}
