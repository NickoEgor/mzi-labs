#pragma once

#include "utility.h"
#include <cstdint>

void belt_init(uint8_t *ks, const uint8_t *k, std::size_t klen);
void belt_encrypt(uint8_t *out, const uint8_t *in, const uint8_t *ks);
void belt_decrypt(uint8_t *out, const uint8_t *in, const uint8_t *ks);
