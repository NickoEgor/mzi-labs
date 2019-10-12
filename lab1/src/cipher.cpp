#include <algorithm>
#include <cassert>
#include <cstring>
#include <iterator>

#include "block.h"
#include "cipher.h"
#include "const.h"

DESCipher::DESCipher(const std::vector<uint8_t> &key) {
  // incorrect key size
  assert(key.size() == 8);

  this->generateSubkeys(key);
}

const std::vector<uint64_t> &DESCipher::GetSubkeys() const {
  return this->subkeys;
}

std::vector<uint8_t> DESCipher::Encrypt(const std::vector<uint8_t> &msg) {
  return this->crypt(msg, true);
}

std::vector<uint8_t> DESCipher::Decrypt(const std::vector<uint8_t> &msg) {
  return this->crypt(msg, false);
}

std::vector<uint8_t> DESCipher::crypt(const std::vector<uint8_t> &msg,
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

void DESCipher::generateSubkeys(const std::vector<uint8_t> &keyBytes) {
  if (!feistelBoxInited) {
    initFeistelBox();
    feistelBoxInited = true;
  }

  // apply PC1 permutation to key
  // uint64_t key = binary.BigEndian.Uint64(keyBytes);
  uint64_t key = 0;
  for (int i = 0; i < 8; i++) {
    key += (static_cast<uint64_t>(keyBytes[i]) << blockSize * (7 - i));
  }

  uint64_t permutedKey = permuteBlock(key, permutedChoice1);

  // rotate halves of permuted key according to the rotation schedule
  std::vector<uint32_t> leftRotations =
      ksRotate(static_cast<uint32_t>(permutedKey >> 28));
  std::vector<uint32_t> rightRotations =
      ksRotate(static_cast<uint32_t>(permutedKey << 4) >> 4);

  // generate subkeys
  for (int8_t i = 0; i < 16; i++) {
    // combine halves to form 56-bit input to PC2
    uint64_t pc2Input = static_cast<uint64_t>(leftRotations[i]) << 28 |
                        static_cast<uint64_t>(rightRotations[i]);
    // apply PC2 permutation to 7 byte input
    this->subkeys[i] = unpack(permuteBlock(pc2Input, permutedChoice2));
  }
}

void DESCipher::encrypt(std::vector<uint8_t> &dst,
                        const std::vector<uint8_t> &src) {
  // input not full block
  assert(src.size() >= blockSize);

  // output not full block
  assert(dst.size() >= blockSize);

  encryptBlock(this->subkeys, dst, src);
}

void DESCipher::decrypt(std::vector<uint8_t> &dst,
                        const std::vector<uint8_t> &src) {
  // input not full block
  assert(src.size() >= blockSize);

  // output not full block
  assert(dst.size() >= blockSize);

  // if (InexactOverlap(dst, blockSize, src, blockSize)) {
  // throw std::length_error("invalid buffer overlap");
  // }

  decryptBlock(this->subkeys, dst, src);
}
