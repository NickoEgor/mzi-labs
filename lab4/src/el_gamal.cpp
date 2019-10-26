#include <cassert>
#include <cmath>
#include <cstdlib>

#include "el_gamal.h"

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

u64 powmod(u64 a, u64 b, u64 p) {
  u64 res = 1;
  while (b)
    if (b & 1)
      res = u64(res * a % p), --b;
    else
      a = u64(a * a % p), b >>= 1;
  return res;
}

u64 primitive_root(u64 p) {
  std::vector<u64> fact;
  u64 phi = p - 1, n = phi;
  for (u64 i = 2; i * i <= n; ++i)
    if (n % i == 0) {
      fact.push_back(i);
      while (n % i == 0)
        n /= i;
    }
  if (n > 1)
    fact.push_back(n);

  for (u64 res = 2; res <= p; ++res) {
    bool ok = true;
    for (std::size_t i = 0; i < fact.size() && ok; ++i)
      ok &= powmod(res, phi / fact[i], p) != 1;
    if (ok)
      return res;
  }
  return -1;
}

u64 get_rand_coprime(u64 p) {
  assert(p - 2 > 1);

  // u64 x = p - 2;
  // while (x > 1) {
  // if (gcd(p - 1, x) == 1) {
  // return x;
  // }
  // x--;
  // }

  while (true) {
    u64 x = 1 + u64(std::rand() / double(p - 2));
    if (gcd(p - 1, x) == 1) {
      return x;
    }
  }

  return -1;
}

std::pair<u64, std::vector<u64>> encrypt(const std::vector<u64> &msg, u64 g,
                                         u64 p, u64 y) {
  std::pair<u64, std::vector<u64>> enc(0, msg.size());

  u64 k = get_rand_coprime(p);
  enc.first = powmod(g, k, p);

  for (std::size_t i = 0; i < msg.size(); ++i) {
    enc.second[i] = (msg[i] * powmod(y, k, p)) % p;
  }
  return enc;
}

std::vector<u64> decrypt(u64 gamma, const std::vector<u64> &enc, u64 p, u64 x) {
  std::vector<u64> dec(enc.size());
  for (std::size_t i = 0; i < enc.size(); ++i) {
    dec[i] = (enc[i] * powmod(gamma, p - 1 - x, p)) % p;
  }
  return dec;
}
