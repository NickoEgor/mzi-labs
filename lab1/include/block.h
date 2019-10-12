#pragma once

#include <cstdint>
#include <utility>
#include <vector>

void cryptBlock(const std::vector<uint64_t> &subkeys, std::vector<uint8_t> &dst,
                std::vector<uint8_t> src, bool decrypt);

// Encrypt one block from src into dst, using the subkeys.
void encryptBlock(std::vector<uint64_t> &subkeys, std::vector<uint8_t> &dst,
                  const std::vector<uint8_t> &src);

// Decrypt one block from src into dst, using the subkeys.
void decryptBlock(std::vector<uint64_t> &subkeys, std::vector<uint8_t> &dst,
                  const std::vector<uint8_t> &src);

// DES Feistel function. feistelBox must be initialized first.
std::pair<uint32_t, uint32_t> feistel(uint32_t l, uint32_t r, uint64_t k0,
                                      uint64_t k1);

// feistelBox[s][16*i+j] contains the output of permutationFunction
// for sBoxes[s][i][j] << 4*(7-s)
extern uint32_t feistelBox[8][64];
extern bool feistelBoxInited;

// general purpose function to perform DES block permutations
uint64_t permuteBlock(uint64_t src, const std::vector<uint8_t> &permutation);

void initFeistelBox();

// permuteInitialBlock is equivalent to the permutation defined
// by initialPermutation.
uint64_t permuteInitialBlock(uint64_t block);

// permuteInitialBlock is equivalent to the permutation defined
// by finalPermutation.
uint64_t permuteFinalBlock(uint64_t block);

// creates 16 28-bit blocks rotated according
// to the rotation schedule
std::vector<uint32_t> ksRotate(uint32_t in);

// Expand 48-bit input to 64-bit, with each 6-bit block padded by extra two bits
// at the top. By doing so, we can have the input blocks (four bits each), and
// the key blocks (six bits each) well-aligned without extra shifts/rotations
// for alignments.
uint64_t unpack(uint64_t x);
