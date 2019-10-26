#include <ctime>
#include <iostream>
#include <utility>
#include <vector>

#include "el_gamal.h"
#include "io.h"
#include "types.h"

int main() {
  std::srand(std::time(nullptr));

  u64 p = prompt_prime("p");
  // u64 p = 65537;
  u64 g = primitive_root(p);
  u64 x = get_rand_coprime(p);
  u64 y = powmod(g, x, p);

  std::cout << "Priv key: " << x << std::endl;
  std::cout << "Pub key (g,p,y): {" << g << ", " << p << ", " << y << "}"
            << std::endl;

  auto msg = prompt_msg();
  std::cout << "Message:" << std::endl;

  auto enc = encrypt(msg, g, p, y);
  std::cout << "Encrypted:" << std::endl;
  print_vector_uint(enc.second);

  auto dec = decrypt(enc.first, enc.second, p, x);
  std::cout << "Decrypted:" << std::endl;
  print_vector_uint(dec);
  std::cout << std::string(dec.begin(), dec.end()) << std::endl;

  return 0;
}
