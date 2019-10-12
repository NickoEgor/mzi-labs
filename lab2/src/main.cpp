#include <iostream>
#include <string>

#include "../include/fileio.h"
#include "../include/modes.h"

int main() {
  Mode m = Mode::GAMMING;

  std::string init_text_file = "init_text.txt";
  std::string encrypted_text_file = "encrypted_text.txt";
  std::string decrypted_text_file = "decrypted_text.txt";

  uint8_t synhro[] = "Hello world!";
  uint8_t keyenc[] = "my_super_secret_code_no_one_knows";

  std::cout << "ALIVE1" << std::endl;
  encrypt_file(m, keyenc, synhro, init_text_file, encrypted_text_file);
  std::cout << "ALIVE2" << std::endl;
  decrypt_file(m, keyenc, synhro, encrypted_text_file, decrypted_text_file);
  std::cout << "ALIVE3" << std::endl;

  return 0;
}
