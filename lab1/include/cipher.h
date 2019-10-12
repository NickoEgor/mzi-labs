#pragma once

#include <cstdint>
#include <vector>

class DESCipher {
  const uint8_t blockSize = 8;
  std::vector<uint64_t> subkeys = std::vector<uint64_t>(16);

  // creates 16 56-bit subkeys from the original key
  void generateSubkeys(const std::vector<uint8_t> &keyBytes);

  std::vector<uint8_t> crypt(const std::vector<uint8_t> &msg, bool encrypt);

  void encrypt(std::vector<uint8_t> &dst, const std::vector<uint8_t> &src);
  void decrypt(std::vector<uint8_t> &dst, const std::vector<uint8_t> &src);

public:
  DESCipher(const std::vector<uint8_t> &key);

  const std::vector<uint64_t> &GetSubkeys() const;

  std::vector<uint8_t> Encrypt(const std::vector<uint8_t> &msg);
  std::vector<uint8_t> Decrypt(const std::vector<uint8_t> &msg);
};
