#include "hmac.hpp"

#include "sha1.hpp"

static const u8 BLOCK_SIZE = 64;
static const u8 HASH_SIZE = 20;

// magic constants
static const u8 IPAD = 0x36;
static const u8 OPAD = 0x5c;

std::vector<u32> hmac(const std::string &msg, std::string key) {
  SHA1 sha;

  // prepare key
  std::vector<uint8_t> key_bytes;
  const uint8_t *raw = nullptr;
  if (key.length() > BLOCK_SIZE) {
    std::vector<u32> key_hashed = sha.eval(key);
    raw = reinterpret_cast<const uint8_t *>(key_hashed.data());
    key_bytes = std::vector<uint8_t>(raw, raw + HASH_SIZE);
    key_bytes.resize(BLOCK_SIZE, 0);
  } else {
    key.resize(BLOCK_SIZE, 0);
    raw = reinterpret_cast<const uint8_t *>(key.data());
    key_bytes = std::vector<uint8_t>(raw, raw + BLOCK_SIZE);
  }

  // prepare XORed keys
  std::string Si(key_bytes.size(), 0);
  for (size_t i = 0; i < key_bytes.size(); ++i) {
    Si[i] = key_bytes[i] ^ IPAD;
  }

  std::string So(key_bytes.size(), 0);
  for (size_t i = 0; i < key_bytes.size(); ++i) {
    So[i] = key_bytes[i] ^ OPAD;
  }

  auto msg_data = msg.data();
  std::vector<uint8_t> msg_bytes(msg_data, msg_data + msg.length());

  std::vector<u32> result;
  std::vector<u32> hBuffer;
  std::string oBuffer(HASH_SIZE, 0);

  size_t i = 0;
  do {
    std::string M(
        msg_bytes.begin() + i,
        msg_bytes.begin() + i +
            std::min(static_cast<size_t>(BLOCK_SIZE), msg_bytes.size() - i));

    hBuffer = sha.eval(Si + M);

    for (size_t i = 0; i < HASH_SIZE / sizeof(u32); i++) {
      oBuffer[4 * i] = static_cast<char>(hBuffer[i] >> 24);
      oBuffer[4 * i + 1] = static_cast<char>(hBuffer[i] >> 16);
      oBuffer[4 * i + 2] = static_cast<char>(hBuffer[i] >> 8);
      oBuffer[4 * i + 3] = static_cast<char>(hBuffer[i]);
    }

    hBuffer = sha.eval(So + oBuffer);
    result.insert(result.end(), hBuffer.begin(), hBuffer.end());

    i += BLOCK_SIZE;
  } while (i < msg_bytes.size());

  return result;
}
