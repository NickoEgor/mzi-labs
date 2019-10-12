#pragma once

#include "modes.h"
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

std::vector<uint8_t> read_str_from_file(const std::string &input);
void write_str_to_file(const std::string &text, const std::string &output);

void encrypt_file(Mode m, const uint8_t *keyenc, const uint8_t *synhro,
                  const std::string &init_text_file,
                  const std::string &encrypted_text_file);

void decrypt_file(Mode m, const uint8_t *keyenc, const uint8_t *synhro,
                  const std::string &encrypted_text_file,
                  const std::string &decrypted_text_file);
