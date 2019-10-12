#include "cipher.h"

class TripleDESCipher {
  DESCipher *d1, *d2, *d3;

  const uint8_t blockSize = 8;

  std::vector<uint8_t> crypt(const std::vector<uint8_t> &msg, bool encrypt);

  void encrypt(std::vector<uint8_t> &dst, const std::vector<uint8_t> &src);
  void decrypt(std::vector<uint8_t> &dst, const std::vector<uint8_t> &src);

public:
  TripleDESCipher(const std::vector<uint8_t> &key);

  std::vector<uint8_t> Encrypt(const std::vector<uint8_t> &msg);
  std::vector<uint8_t> Decrypt(const std::vector<uint8_t> &msg);

  ~TripleDESCipher();
};
