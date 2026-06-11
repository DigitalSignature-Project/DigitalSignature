#pragma once
#include <vector>
#include <string>

namespace digisign {

/**
 * @brief Pads message according to Keccak/SHA‑3 multi‑rate padding rule.
 *
 * Padding rule:
 *   • append domain separation byte (e.g. 0x06 for SHA‑3)
 *   • append zeros until (len % rate) == rate − 1
 *   • append final 0x80
 *
 * This is the Keccak "pad10*1" rule with domain separation.
 */
std::vector<uint8_t> sha3_pad(const std::vector<uint8_t>& message,
                              size_t rate, uint8_t domain);

/**
 * @brief Absorbs one block of input into the Keccak state A.
 *
 * The block is interpreted as little‑endian 64‑bit lanes.
 */
void absorb_block(uint64_t A[5][5], const uint8_t* block, size_t rate);

/**
 * @brief SHA3‑256 hash function.
 */
std::vector<uint8_t> sha3_256(const std::vector<uint8_t>& msg);

/**
 * @brief SHA3‑256 returning hex string.
 */
std::string sha3_256(const std::string& message);

/**
 * @brief SHA3‑512 hash function.
 */
std::vector<uint8_t> sha3_512(const std::vector<uint8_t>& msg);

/**
 * @brief SHA3‑512 returning hex string.
 */
std::string sha3_512(const std::string& message);

/**
 * @brief Keccak‑f[1600] permutation (24 rounds).
 */
void keccak_f(uint64_t A[5][5]);

/**
 * @brief Iota step: adds round constant to A[0][0].
 */
void iota(uint64_t A[5][5], int round);

/**
 * @brief Chi step: non‑linear substitution layer.
 */
void chi(uint64_t A[5][5]);

/**
 * @brief Pi step: permutes lane positions.
 */
void pi(uint64_t A[5][5]);

/**
 * @brief Rho step: rotates each lane by a fixed offset.
 */
void rho(uint64_t A[5][5]);

/**
 * @brief Theta step: mixes columns.
 */
void theta(uint64_t A[5][5]);

/**
 * @brief 64‑bit left rotation.
 */
uint64_t rotl(uint64_t x, int n);

}
