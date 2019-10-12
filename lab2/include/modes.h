#pragma once

#include "belt.h"
#include "utility.h"
#include <cstdint>

enum class Mode { PLAIN, BLOCK, GAMMING, COUNTER };

void encrypt_plain(uint8_t *outenc, const uint8_t *inenc, const uint8_t *ks,
                   const int32_t len);
void decrypt_plain(uint8_t *outdec, const uint8_t *outenc, const uint8_t *ks,
                   const int32_t len);

void encrypt_block(uint8_t *outenc, const uint8_t *inenc, const uint8_t *ks,
                   const int32_t len, const uint8_t *s);
void decrypt_block(uint8_t *outdec, const uint8_t *outenc, const uint8_t *ks,
                   const int32_t len, const uint8_t *s);

void encrypt_gamming(uint8_t *outenc, const uint8_t *inenc, const uint8_t *ks,
                     const int32_t len, const uint8_t *s);
void decrypt_gamming(uint8_t *outdec, const uint8_t *outenc, const uint8_t *ks,
                     const int32_t len, const uint8_t *s);

void encrypt_counter(uint8_t *outenc, const uint8_t *inenc, const uint8_t *ks,
                     const int32_t len, const uint8_t *s);
void decrypt_counter(uint8_t *outdec, const uint8_t *outenc, const uint8_t *ks,
                     const int32_t len, const uint8_t *s);
