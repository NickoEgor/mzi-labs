#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "types.hpp"

class SHA1 {
public:
  SHA1();
  std::vector<u32> eval(const std::string &s);
  std::vector<u32> eval_file(const std::string &filename);

private:
  std::vector<u32> eval(std::istream &s);

  u32 digest[5];
  std::string buffer;
  u64 transforms;
};
