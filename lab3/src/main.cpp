#include <string>
#include <vector>

#include "io.h"
#include "rsa.h"
#include "types.h"

int main() {
  // u64 p = 65537;
  // u64 q = 523;
  u64 p = prompt_prime("p"); // 65537
  u64 q = prompt_prime("q"); // 523

  u64 n = p * q;
  u64 t = (p - 1) * (q - 1);
  u64 e = calc_e(t);
  u64 d = calc_d(e, t);

  std::cout << "Pub key (e,n): {" << e << ", " << n << "}" << std::endl;
  std::cout << "Priv key (d,n): {" << d << ", " << n << "}" << std::endl;

  auto msg = prompt_msg();
  std::cout << "Message: " << std::endl;
  print_vector_uint(msg);

  auto enc = encrypt(msg, e, n);
  std::cout << "Encrypted: " << std::endl;
  print_vector_uint(enc);

  auto dec = decrypt(enc, d, n);
  std::string dec_msg(dec.begin(), dec.end());
  std::cout << "Decrypted: " << std::endl;
  print_vector_uint(dec);
  std::cout << dec_msg << std::endl;

  return 0;
}
