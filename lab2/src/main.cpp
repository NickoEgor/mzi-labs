#include <iomanip>
#include <iostream>
#include <string>

#include "../include/fileio.h"
#include "../include/modes.h"

int main() {
  // --------------------------------------------
  // Mode m = Mode::PLAIN;

  // std::string init_text_file = "init_text.txt";
  // std::string encrypted_text_file = "encrypted_text.txt";
  // std::string decrypted_text_file = "decrypted_text.txt";

  // uint8_t synhro[] = "Hello world!";
  // uint8_t keyenc[] = "my_super_secret_code_no_one_knows";

  // encrypt_file(m, keyenc, synhro, init_text_file, encrypted_text_file);
  // decrypt_file(m, keyenc, synhro, encrypted_text_file, decrypted_text_file);
  // --------------------------------------------

  Mode m = Mode::COUNTER;

  std::vector<uint64_t> key, inp, sync;
  switch (m) {
  case Mode::PLAIN:
    inp = {0xB194BAC8, 0x0A08F53B, 0x366D008E, 0x584A5DE4};
    key = {0xE9DEE72C, 0x8F0C0FA6, 0x2DDB49F4, 0x6F739647,
           0x06075316, 0xED247A37, 0x39CBA383, 0x03A98BF6};
    // encoded: 69CCA1C9 3557C9E3 D66BC3E0 FA88FA6E
    break;
  case Mode::BLOCK:
    inp = {0xB194BAC8, 0x0A08F53B, 0x366D008E, 0x584A5DE4,
           0x8504FA9D, 0x1BB6C7AC, 0x252E72C2, 0x02FDCE0D,
           0x5BE3D612, 0x17B96181, 0xFE6786AD, 0x716B890B};
    key = {0xE9DEE72C, 0x8F0C0FA6, 0x2DDB49F4, 0x6F739647,
           0x06075316, 0xED247A37, 0x39CBA383, 0x03A98BF6};
    sync = {0xBE329713, 0x43FC9A48, 0xA02A885F, 0x194B09A1};
    // encoded:
    // 10116EFA E6AD58EE 14852E11 DA1B8A74 5CF2480E 8D03F1C1 9492E53E D3A70F60
    // 657C1EE8 C0E0AE5B 58388BF8 A68E3309
    break;
  case Mode::GAMMING:
    inp = {0xB194BAC8, 0x0A08F53B, 0x366D008E, 0x584A5DE4,
           0x8504FA9D, 0x1BB6C7AC, 0x252E72C2, 0x02FDCE0D,
           0x5BE3D612, 0x17B96181, 0xFE6786AD, 0x716B890B};
    key = {0xE9DEE72C, 0x8F0C0FA6, 0x2DDB49F4, 0x6F739647,
           0x06075316, 0xED247A37, 0x39CBA383, 0x03A98BF6};
    sync = {0xBE329713, 0x43FC9A48, 0xA02A885F, 0x194B09A1};
    // encoded:
    // C31E490A 90EFA374 626CC99E 4B7B8540 A6E48685 464A5A06 849C9CA7 69A1B0AE
    break;
  case Mode::COUNTER:
    inp = {0xB194BAC8, 0x0A08F53B, 0x366D008E, 0x584A5DE4,
           0x8504FA9D, 0x1BB6C7AC, 0x252E72C2, 0x02FDCE0D,
           0x5BE3D612, 0x17B96181, 0xFE6786AD, 0x716B890B};
    key = {0xE9DEE72C, 0x8F0C0FA6, 0x2DDB49F4, 0x6F739647,
           0x06075316, 0xED247A37, 0x39CBA383, 0x03A98BF6};
    sync = {0xBE329713, 0x43FC9A48, 0xA02A885F, 0x194B09A1};
    // encoded:
    // 52C9AF96 FF50F644 35FC43DE F56BD797 D5B5B1FF 79FB4125 7AB9CDF6 E63E81F8
    break;
  }

  std::vector<uint8_t> input;
  for (auto &v : inp) {
    input.push_back(static_cast<uint8_t>(v >> 24));
    input.push_back(static_cast<uint8_t>(v >> 16));
    input.push_back(static_cast<uint8_t>(v >> 8));
    input.push_back(static_cast<uint8_t>(v));
  }

  std::vector<uint8_t> keyenc;
  for (auto &v : key) {
    keyenc.push_back(static_cast<uint8_t>(v >> 24));
    keyenc.push_back(static_cast<uint8_t>(v >> 16));
    keyenc.push_back(static_cast<uint8_t>(v >> 8));
    keyenc.push_back(static_cast<uint8_t>(v));
  }

  std::vector<uint8_t> synchro;
  for (auto &v : sync) {
    synchro.push_back(static_cast<uint8_t>(v >> 24));
    synchro.push_back(static_cast<uint8_t>(v >> 16));
    synchro.push_back(static_cast<uint8_t>(v >> 8));
    synchro.push_back(static_cast<uint8_t>(v));
  }

  uint8_t ks[32] = {0};
  belt_init(ks, keyenc.data(), 32);

  // encoding
  std::vector<uint8_t> out(input.size(), 0);
  switch (m) {
  case Mode::PLAIN:
    encrypt_plain(out.data(), input.data(), ks, input.size());
    break;
  case Mode::BLOCK:
    encrypt_block(out.data(), input.data(), ks, input.size(), synchro.data());
    break;
  case Mode::GAMMING:
    encrypt_gamming(out.data(), input.data(), ks, input.size(), synchro.data());
    break;
  case Mode::COUNTER:
    encrypt_counter(out.data(), input.data(), ks, input.size(), synchro.data());
    break;
  }

  std::cout << "Encoded: ";
  for (auto &v : out) {
    std::cout << std::hex << std::setfill('0') << std::setw(2)
              << static_cast<uint32_t>(v) << " ";
  }
  std::cout << std::endl;

  // decoding
  std::vector<uint8_t> decod(input.size(), 0);
  switch (m) {
  case Mode::PLAIN:
    decrypt_plain(decod.data(), out.data(), ks, out.size());
    break;
  case Mode::BLOCK:
    decrypt_block(decod.data(), out.data(), ks, out.size(), synchro.data());
    break;
  case Mode::GAMMING:
    decrypt_gamming(decod.data(), out.data(), ks, out.size(), synchro.data());
    break;
  case Mode::COUNTER:
    decrypt_counter(decod.data(), out.data(), ks, out.size(), synchro.data());
    break;
  }

  std::cout << "Decoded: ";
  for (auto &v : decod) {
    std::cout << std::hex << std::setfill('0') << std::setw(2)
              << static_cast<uint32_t>(v) << " ";
  }
  std::cout << std::endl;

  return 0;
}
