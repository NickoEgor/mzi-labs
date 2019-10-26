#pragma once

#include <cmath>
#include <vector>

#include "types.h"

u64 gcd(u64 a, u64 b);
bool is_prime(u64 n);
u64 calc_e(u64 t);
u64 calc_d(u64 e, u64 t);
std::vector<u64> encrypt(const std::vector<u64> &msg, u64 e, u64 n);
std::vector<u64> decrypt(const std::vector<u64> &enc, u64 d, u64 n);
