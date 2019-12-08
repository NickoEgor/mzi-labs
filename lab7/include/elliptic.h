#pragma once

#include <tuple>
#include <utils.h>

template <typename T, T mod> class ModNumber {
public:
  ModNumber() : value(0) {}

  ModNumber(T value_) : value(validate(value_)) {}

  void operator=(const ModNumber &other) { value = validate(other.value); }

  void operator=(T value_) { value = validate(value_); }

  // binary
  ModNumber operator+(const ModNumber &other) const {
    T v = (this->value + other.value) % mod;
    ModNumber res(v);

    return res;
  }

  ModNumber operator-(const ModNumber &other) const {
    T v = (this->value - other.value + mod) % mod;
    ModNumber res(v);

    return res;
  }

  ModNumber operator*(const ModNumber &other) const {
    T v = (this->value * other.value) % mod;
    ModNumber res(v);

    return res;
  }

  ModNumber operator/(const ModNumber &other) const {
    assert(other.value != 0);

    T v = BinPow(other.value, mod - 2, mod);
    ModNumber res(v);

    res = res * (*this);

    return res;
  }

  // unary
  ModNumber operator+() const { return *this; }

  ModNumber operator-() const {
    ModNumber res = -this->value;

    return res;
  }

  // comparison
  bool operator==(const ModNumber &other) const {
    return (this->value == other.value);
  }

  // cast
  explicit operator T() const { return this->value; }

  //  private:
  T validate(T v) {
    T res = ((v % mod) + mod) % mod;

    return res;
  }

  T value;
};

template <typename T, T mod>
std::ostream &operator<<(std::ostream &st, const ModNumber<T, mod> &n) {
  st << "(" << n.value << " mod " << mod << ")";

  return st;
}

template <class Number> struct ModPoint {
  ModPoint() : x(0), y(0) {}

  ModPoint(Number x_, Number y_) : x(x_), y(y_) {}

  void operator=(const ModPoint &other) {
    x = other.x;
    y = other.y;
  }

  Number x, y;
};

template <class T>
std::ostream &operator<<(std::ostream &st, const ModPoint<T> &p) {
  st << "[" << p.x << ", " << p.y << "]";

  return st;
}

template <class Number, class Point> class EllipticCurve {
public:
  EllipticCurve(Number a_, Number b_) : a(a_), b(b_) {}

  Point Add(const Point &p2, const Point &p1) const {
    if (isnull(p1) || isnull(p2)) {
      return (isnull(p1) ? p2 : p1);
    }

    Number m;
    if (p1.x == p2.x) {
      if (p1.y == p2.y) {
        m = (p1.x * p1.x * 3 + a) / (p1.y * 2);
      } else {
        Point res(0, 0);
        return res;
      }
    } else {
      Number dy = (p2.y - p1.y);
      Number dx = (p2.x - p1.x);

      m = dy / dx;
    }

    Number x = m * m - p2.x - p1.x;
    Number y = p1.y + m * (x - p1.x);

    Point res(x, -y);

    return res;
  }

  bool isnull(const Point &p) const { return (p.x == 0 && p.y == 0); }

  // private:
  Number a, b;
};

template <class Point, class Curve>
Point BinPow(const Point &base, int st, const Curve &curve) {
  assert(st > 0);
  if (st == 1) {
    return base;
  }

  Point res;
  if (st % 2 == 0) {
    res = BinPow(base, st / 2, curve);
    res = curve.Add(res, res);
  } else {
    res = BinPow(base, st - 1, curve);
    res = curve.Add(res, base);
  }

  return res;
}
