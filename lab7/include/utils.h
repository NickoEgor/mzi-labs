#pragma once

#include <array>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

std::string join(const std::vector<std::string> &strs, std::string delim);

uint64_t BinPow(uint64_t base, uint64_t st, uint64_t mod);
uint64_t gcd(uint64_t a, uint64_t b);
uint64_t gcdex(int64_t a, int64_t b, int64_t &x, int64_t &y);
