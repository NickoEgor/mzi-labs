#include <cstdint>
#include <iostream>
#include <string>

#include "block.h"
#include "triple.h"

std::vector<uint8_t> prompt_key(int len = 8) {
  std::string key;
  std::cout << "Enter key:" << std::endl;
  std::getline(std::cin, key);
  key.resize(len, 0);
  return std::vector<uint8_t>(key.begin(), key.end());
}

std::vector<uint8_t> prompt_msg() {
  std::string msg;
  std::cout << "Enter msg:" << std::endl;
  std::getline(std::cin, msg);

  int remain = msg.size() % 8;
  if (remain > 0) {
    int whole = msg.size() / 8;
    msg.resize((whole + 1) * 8, 0);
  }

  return std::vector<uint8_t>(msg.begin(), msg.end());
}

int main() {
  std::vector<uint8_t> src = prompt_msg();
  std::vector<uint8_t> key = prompt_key(24);

  TripleDESCipher dc(key);

  std::vector<uint8_t> dst1 = dc.Encrypt(src);
  std::cout << "enc: " << std::hex;
  for (std::size_t i = 0; i < dst1.size(); i++) {
    std::cout << (uint64_t)(dst1[i]) << " ";
  }
  std::cout << std::dec << std::endl;

  std::vector<uint8_t> dst2 = dc.Decrypt(dst1);
  std::cout << "dec: " << std::hex;
  for (std::size_t i = 0; i < dst2.size(); i++) {
    std::cout << (uint64_t)(dst2[i]) << " ";
  }
  std::cout << std::dec << std::endl;

  std::cout << "dec(text): ";
  for (std::size_t i = 0; i < dst2.size(); i++) {
    std::cout << (char)(dst2[i]);
  }
  std::cout << std::endl;

  return 0;
}
