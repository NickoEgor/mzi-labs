#include "../include/fileio.h"
#include <iostream>

std::vector<uint8_t> read_str_from_file(const std::string &input) {
  std::ifstream t(input);
  t.seekg(0, std::ios::end);
  size_t size = t.tellg();
  std::string buffer(size, ' ');
  t.seekg(0);
  t.read(&buffer[0], size);
  return {buffer.begin(), buffer.end()};
}

void write_str_to_file(const std::vector<uint8_t> &text,
                       const std::string &output) {
  std::ofstream t(output);
  t << std::string(text.begin(), text.end());
}

void encrypt_file(Mode m, const uint8_t *keyenc, const uint8_t *synhro,
                  const std::string &init_text_file,
                  const std::string &encrypted_text_file) {
  std::vector<uint8_t> inenc = read_str_from_file(init_text_file);

  std::vector<uint8_t> outenc(inenc.size() + 10, 0);
  uint8_t ks[32] = {0};

  belt_init(ks, keyenc, 32);
  switch (m) {
  case Mode::PLAIN:
    encrypt_plain(outenc.data(), inenc.data(), ks, inenc.size());
    break;
  case Mode::BLOCK:
    encrypt_block(outenc.data(), inenc.data(), ks, inenc.size(), synhro);
    break;
  case Mode::GAMMING:
    encrypt_gamming(outenc.data(), inenc.data(), ks, inenc.size(), synhro);
    break;
  case Mode::COUNTER:
    encrypt_counter(outenc.data(), inenc.data(), ks, inenc.size(), synhro);
    break;
  }

  write_str_to_file(outenc, encrypted_text_file);
}

void decrypt_file(Mode m, const uint8_t *keyenc, const uint8_t *synhro,
                  const std::string &encrypted_text_file,
                  const std::string &decrypted_text_file) {
  std::vector<uint8_t> outenc = read_str_from_file(encrypted_text_file);

  std::vector<uint8_t> outdec(outenc.size(), 0);
  uint8_t ks[32] = {0};

  belt_init(ks, keyenc, 32);
  switch (m) {
  case Mode::PLAIN:
    decrypt_plain(outdec.data(), outenc.data(), ks, outenc.size());
    break;
  case Mode::BLOCK:
    decrypt_block(outdec.data(), outenc.data(), ks, outenc.size(), synhro);
    break;
  case Mode::GAMMING:
    decrypt_gamming(outdec.data(), outenc.data(), ks, outenc.size(), synhro);
    break;
  case Mode::COUNTER:
    decrypt_counter(outdec.data(), outenc.data(), ks, outenc.size(), synhro);
    break;
  }

  write_str_to_file(outdec, decrypted_text_file);
}
