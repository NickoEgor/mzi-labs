#include <elliptic.h>
#include <iostream>
#include <set>
#include <tuple>
#include <utility>
#include <utils.h>

constexpr uint64_t MOD = 1000000007LL;
// constexpr int64_t MOD = 127;

typedef ModNumber<int64_t, MOD> Number;
typedef ModPoint<Number> Point;
typedef EllipticCurve<Number, Point> Curve;

int main() {
  // public data
  Number a = 1;
  Number b = 1;
  Curve curve(a, b);
  Point P(5, 17);

  // private data
  int alice_private_key = 12345678;
  int bob_private_key = 87654321;

  std::cout << "Private key A = " << alice_private_key << std::endl;
  std::cout << "Private key B = " << bob_private_key << std::endl;

  std::cout << "P = " << P << std::endl;

  // Alice -> Bob
  Point alice_public_key = BinPow(P, alice_private_key, curve);
  std::cout << "Public key A = " << alice_public_key << std::endl;

  // Bob -> Alice
  Point bob_public_key = BinPow(P, bob_private_key, curve);
  std::cout << "Public key B = " << bob_public_key << std::endl;
  std::cout << std::endl;

  Point common_key;
  // Alice
  Point alice_common_key = BinPow(bob_public_key, alice_private_key, curve);
  std::cout << "Common key A = " << alice_common_key << std::endl;

  // Bob
  Point bob_common_key = BinPow(alice_public_key, bob_private_key, curve);
  std::cout << "Common key B = " << bob_common_key << std::endl;

  return 0;
}
