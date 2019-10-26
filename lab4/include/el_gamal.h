#pragma once

#include <utility>
#include <vector>

#include "types.h"

u64 gcd(u64 a, u64 b);
bool is_prime(u64 n);
u64 powmod(u64 a, u64 b, u64 p);
u64 primitive_root(u64 p);
u64 get_rand_coprime(u64 p);
std::pair<u64, std::vector<u64>> encrypt(const std::vector<u64> &msg, u64 g,
                                         u64 p, u64 y);
std::vector<u64> decrypt(u64 gamma, const std::vector<u64> &enc, u64 p, u64 x);
