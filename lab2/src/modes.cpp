#include "../include/modes.h"

// прямая замена
void encrypt_plain(uint8_t *outenc, const uint8_t *inenc, const uint8_t *ks,
                   const int32_t len) {
  uint8_t buffer[16] = {0};
  uint8_t outchunk[16] = {0};
  for (int i = 0; i < (len + 15) / 16; ++i) {
    store128(buffer, inenc + i * 16);
    belt_encrypt(outchunk, buffer, ks);
    store128(outenc + i * 16, outchunk);
  }
}

void decrypt_plain(uint8_t *outdec, const uint8_t *outenc, const uint8_t *ks,
                   const int32_t len) {
  uint8_t buffer[16] = {0};
  uint8_t outchunk[16] = {0};
  for (int i = 0; i < (len + 15) / 16; ++i) {
    store128(buffer, outenc + i * 16);
    belt_decrypt(outchunk, buffer, ks);
    store128(outdec + i * 16, outchunk);
  }
}

// сцепление блоков
void encrypt_block(uint8_t *outenc, const uint8_t *inenc, const uint8_t *ks,
                   const int32_t len, const uint8_t *s) {
  uint8_t buffer[16] = {0};
  uint8_t outchunk[16] = {0};
  uint8_t synhro[16] = {0};
  belt_encrypt(synhro, s, ks);
  for (int i = 0; i < (len + 15) / 16; ++i) {
    store128(buffer, inenc + i * 16);

    for (std::size_t j = 0; j < 16; ++j) {
      buffer[j] = buffer[j] ^ synhro[j];
    }
    belt_encrypt(outchunk, buffer, ks);

    store128(outenc + i * 16, outchunk);
    store128(synhro, outchunk);
  }
}

void decrypt_block(uint8_t *outdec, const uint8_t *outenc, const uint8_t *ks,
                   const int32_t len, const uint8_t *s) {
  uint8_t buffer[16] = {0};
  uint8_t outchunk[16] = {0};
  uint8_t synhro[16] = {0};
  belt_encrypt(synhro, s, ks);
  for (int i = 0; i < (len + 15) / 16; ++i) {
    store128(buffer, outenc + i * 16);

    belt_decrypt(outchunk, buffer, ks);
    for (std::size_t j = 0; j < 16; ++j) {
      outchunk[j] = outchunk[j] ^ synhro[j];
    }

    store128(outdec + i * 16, outchunk);
    store128(synhro, buffer);
  }
}

// гаммирование
void encrypt_gamming(uint8_t *outenc, const uint8_t *inenc, const uint8_t *ks,
                     const int32_t len, const uint8_t *s) {
  uint8_t buffer[16] = {0};
  uint8_t outchunk[16] = {0};
  uint8_t synhro[16] = {0};
  uint8_t ns[16] = {0};
  store128(ns, s);
  for (int i = 0; i < (len + 15) / 16; ++i) {
    belt_encrypt(synhro, ns, ks);
    store128(buffer, inenc + i * 16);

    for (std::size_t j = 0; j < 16; ++j) {
      outchunk[j] = buffer[j] ^ synhro[j];
    }

    store128(outenc + i * 16, outchunk);
    store128(ns, outchunk);
  }
}

void decrypt_gamming(uint8_t *outdec, const uint8_t *outenc, const uint8_t *ks,
                     const int32_t len, const uint8_t *s) {
  uint8_t buffer[16] = {0};
  uint8_t outchunk[16] = {0};
  uint8_t synhro[16] = {0};
  uint8_t ns[16] = {0};
  store128(ns, s);
  for (int i = 0; i < (len + 15) / 16; ++i) {
    belt_encrypt(synhro, ns, ks);
    store128(buffer, outenc + i * 16);

    for (std::size_t j = 0; j < 16; ++j) {
      outchunk[j] = buffer[j] ^ synhro[j];
    }

    store128(outdec + i * 16, outchunk);
    store128(ns, buffer);
  }
}

// счётчик
void encrypt_counter(uint8_t *outenc, const uint8_t *inenc, const uint8_t *ks,
                     const int32_t len, const uint8_t *s) {
  uint8_t buffer[16] = {0};
  uint8_t outchunk[16] = {0};
  uint8_t synhro[16] = {0};
  uint8_t ns[16] = {0};
  belt_encrypt(ns, s, ks);
  for (int i = 0; i < (len + 15) / 16; ++i) {
    belt_encrypt(synhro, ns, ks);
    square_plus(ns, synhro);
    store128(buffer, inenc + i * 16);

    for (std::size_t j = 0; j < 16; ++j) {
      outchunk[j] = buffer[j] ^ ns[j];
    }

    store128(outenc + i * 16, outchunk);
  }
}

void decrypt_counter(uint8_t *outdec, const uint8_t *outenc, const uint8_t *ks,
                     const int32_t len, const uint8_t *s) {
  encrypt_counter(outdec, outenc, ks, len, s);
}
