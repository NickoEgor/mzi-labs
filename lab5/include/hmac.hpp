#pragma once

#include <string>
#include <vector>

#include "types.hpp"

std::vector<u32> hmac(const std::string &msg, std::string key);
