#include "triple.h"
#include "block.h"
#include <cassert>

TripleDESCipher::TripleDESCipher(const std::vector<uint8_t> &key) {
  // incorrect key size
  assert(key.size() == 24);

  d1 = new DESCipher(std::vector<uint8_t>(key.begin(), key.begin() + 8));
  d2 = new DESCipher(std::vector<uint8_t>(key.begin() + 8, key.begin() + 16));
  d3 = new DESCipher(std::vector<uint8_t>(key.begin() + 16, key.end()));
}

TripleDESCipher::~TripleDESCipher() {
  delete d1;
  delete d2;
  delete d3;
}

std::vector<uint8_t> TripleDESCipher::Encrypt(const std::vector<uint8_t> &msg) {
  return this->crypt(msg, true);
}

std::vector<uint8_t> TripleDESCipher::Decrypt(const std::vector<uint8_t> &msg) {
  return this->crypt(msg, false);
}

std::vector<uint8_t> TripleDESCipher::crypt(const std::vector<uint8_t> &msg,
                                            bool encrypt) {
  assert(msg.size() > 0 && msg.size() % blockSize == 0);

  std::size_t blocks = msg.size() / blockSize;

  std::vector<uint8_t> res(msg.size());

  std::vector<uint8_t> resBuffer(blockSize);
  std::vector<uint8_t> msgBuffer(blockSize);

  for (std::size_t i = 0; i < blocks; i++) {
    std::fill(resBuffer.begin(), resBuffer.end(), 0);
    std::copy(msg.begin() + i * blockSize, msg.begin() + (i + 1) * blockSize,
              msgBuffer.begin());

    if (encrypt)
      this->encrypt(resBuffer, msgBuffer);
    else
      this->decrypt(resBuffer, msgBuffer);

    std::copy(resBuffer.begin(), resBuffer.end(), res.begin() + i * blockSize);
  }
  return res;
}

void TripleDESCipher::encrypt(std::vector<uint8_t> &dst,
                              const std::vector<uint8_t> &src) {
  // input not full block
  assert(src.size() >= blockSize);

  // output not full block
  assert(dst.size() >= blockSize);

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

  auto sub1 = d1->GetSubkeys();
  auto sub2 = d2->GetSubkeys();
  auto sub3 = d3->GetSubkeys();

  for (int i = 0; i < 8; i++) {
    auto pair = feistel(left, right, sub1[2 * i], sub1[2 * i + 1]);
    left = pair.first;
    right = pair.second;
  }
  for (int i = 0; i < 8; i++) {
    auto pair = feistel(right, left, sub2[15 - 2 * i], sub2[15 - (2 * i + 1)]);
    right = pair.first;
    left = pair.second;
  }
  for (int i = 0; i < 8; i++) {
    auto pair = feistel(left, right, sub3[2 * i], sub3[2 * i + 1]);
    left = pair.first;
    right = pair.second;
  }

  left = (left << 31) | (left >> 1);
  right = (right << 31) | (right >> 1);

  uint64_t preOutput =
      (static_cast<uint64_t>(right) << 32) | static_cast<uint64_t>(left);

  uint64_t out = permuteFinalBlock(preOutput);
  assert(dst.size() == 8);
  for (int i = 0; i < 8; i++) {
    dst[i] = (out >> 8 * (7 - i));
  }
}

void TripleDESCipher::decrypt(std::vector<uint8_t> &dst,
                              const std::vector<uint8_t> &src) {
  // input not full block
  assert(src.size() >= blockSize);

  // output not full block
  assert(dst.size() >= blockSize);

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

  auto sub1 = d1->GetSubkeys();
  auto sub2 = d2->GetSubkeys();
  auto sub3 = d3->GetSubkeys();

  for (int i = 0; i < 8; i++) {
    auto pair = feistel(left, right, sub3[15 - 2 * i], sub3[15 - (2 * i + 1)]);
    left = pair.first;
    right = pair.second;
  }
  for (int i = 0; i < 8; i++) {
    auto pair = feistel(right, left, sub2[2 * i], sub2[2 * i + 1]);
    right = pair.first;
    left = pair.second;
  }
  for (int i = 0; i < 8; i++) {
    auto pair = feistel(left, right, sub1[15 - 2 * i], sub1[15 - (2 * i + 1)]);
    left = pair.first;
    right = pair.second;
  }

  left = (left << 31) | (left >> 1);
  right = (right << 31) | (right >> 1);

  uint64_t preOutput =
      (static_cast<uint64_t>(right) << 32) | static_cast<uint64_t>(left);

  uint64_t out = permuteFinalBlock(preOutput);
  assert(dst.size() == 8);
  for (int i = 0; i < 8; i++) {
    dst[i] = (out >> 8 * (7 - i));
  }
}
