#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "hmac.hpp"

std::string prompt(const std::string &key) {
  std::string value;
  std::cout << "Enter " << key << ":" << std::endl;
  std::getline(std::cin, value);
  return value;
}

template <typename U> void print_vector_uint(const std::vector<U> &vec) {
  for (const auto &v : vec) {
    std::cout << std::hex << std::setw(sizeof(U) * 2) << std::setfill('0')
              << static_cast<unsigned long long>(v) << " ";
  }
  std::cout << std::dec << std::endl;
}

void print_string_hex(const std::string &str) {
  for (const auto &v : str) {
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<unsigned long long>(v) << " ";
  }
  std::cout << std::dec << std::endl;
}

int main() {
  std::string msg = prompt("message");
  std::string key = prompt("key");
  auto enc = hmac(msg, key);
  std::cout << "Message hash:" << std::endl;
  print_vector_uint(enc);

  return 0;
}
