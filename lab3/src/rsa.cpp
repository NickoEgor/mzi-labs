#include "rsa.h"

u64 gcd(u64 a, u64 b) {
  if (a < b) {
    u64 t = a;
    a = b;
    b = t;
  }

  while (true) {
    u64 r = a % b;
    if (r == 0)
      return b;
    a = b;
    b = r;
  }
}

bool is_prime(u64 n) {
  u64 root = static_cast<u64>(floor(sqrt(static_cast<long double>(n))));

  for (u64 i = 2; i <= root; ++i) {
    if (n % i == 0)
      return false;
  }

  return true;
}

u64 calc_e(u64 t) {
  for (u64 e = 2; e < t; e++) {
    if (gcd(e, t) == 1) {
      return e;
    }
  }

  return -1;
}

u64 calc_d(u64 e, u64 t) {
  u64 k = 1;
  while (true) {
    k += t;
    if (k % e == 0) {
      return k / e;
    }
  }
}

std::vector<u64> encrypt(const std::vector<u64> &msg, u64 e, u64 n) {
  std::vector<u64> enc(msg.size(), 0);
  for (std::size_t k = 0; k < msg.size(); ++k) {
    u64 res = msg[k];
    for (u64 i = 0; i < e - 1; ++i) {
      res *= msg[k];
      res %= n;
    }
    enc[k] = res;
  }
  return enc;
}

std::vector<u64> decrypt(const std::vector<u64> &enc, u64 d, u64 n) {
  std::vector<u64> dec(enc.size(), 0);
  for (std::size_t k = 0; k < enc.size(); ++k) {
    u64 res = enc[k];
    for (u64 i = 0; i < d - 1; ++i) {
      res *= enc[k];
      res %= n;
    }
    dec[k] = res;
  }
  return dec;
}
