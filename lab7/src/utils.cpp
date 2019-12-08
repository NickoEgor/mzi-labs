#include <utils.h>

std::string join(const std::vector<std::string> &strs, std::string delim) {
  std::string result_str = "";
  for (size_t i = 0; i < strs.size(); ++i) {
    if (i != 0) {
      result_str += delim;
    }
    result_str += strs[i];
  }

  return result_str;
}

uint64_t BinPow(uint64_t base, uint64_t st, uint64_t mod) {
  if (st == 0) {
    return 1;
  }

  uint64_t result;
  if (st % 2 == 0) {
    uint64_t tmp = BinPow(base, st / 2, mod);
    result = (tmp * tmp) % mod;
  } else {
    result = (base * BinPow(base, st - 1, mod)) % mod;
  }

  return result;
}

uint64_t gcd(uint64_t a, uint64_t b) {
  if (b == 0) {
    return a;
  } else {
    return gcd(b, a % b);
  }
}

uint64_t gcdex(int64_t a, int64_t b, int64_t &x, int64_t &y) {
  if (a == 0) {
    x = 0;
    y = 1;
    return b;
  }
  int64_t x1, y1;
  uint64_t d = gcdex(b % a, a, x1, y1);
  x = y1 - (b / a) * x1;
  y = x1;
  return d;
}
