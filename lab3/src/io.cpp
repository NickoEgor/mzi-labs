#include "io.h"
#include "rsa.h"

#include <iomanip>

u64 prompt_prime(const char *name) {
  u64 num = 0;
  while (true) {
    std::cout << "Enter " << name << ":" << std::endl;
    std::cin >> num;

    std::cin.clear();
    std::cin.ignore(10000, '\n');

    if (num != 0 && is_prime(num))
      break;
    std::cout << "Number is not prime. Try again" << std::endl;
  }
  return num;
}

std::vector<u64> prompt_msg() {
  std::string msg;
  std::cout << "Enter message:" << std::endl;
  std::getline(std::cin, msg);
  return {msg.begin(), msg.end()};
}

void print_vector_uint(const std::vector<u64> &vec) {
  for (const auto &v : vec) {
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<unsigned long long int>(v) << " ";
  }
  std::cout << std::dec << std::endl;
}
