#pragma once
#include <vector>
#include <string>

namespace digisign {

/**
 * @brief Right rotation (circular shift) of 32‑bit word.
 */
uint32_t rotr(uint32_t x, int n);

/**
 * @brief SHA‑256 small sigma₀ function:
 *        σ₀(x) = ROTR⁷(x) ⊕ ROTR¹⁸(x) ⊕ (x >> 3)
 */
uint32_t sigma0(uint32_t x);

/**
 * @brief SHA‑256 small sigma₁ function:
 *        σ₁(x) = ROTR¹⁷(x) ⊕ ROTR¹⁹(x) ⊕ (x >> 10)
 */
uint32_t sigma1(uint32_t x);

/**
 * @brief SHA‑256 choice function:
 *        Ch(e,f,g) = (e ∧ f) ⊕ (~e ∧ g)
 */
uint32_t ch(uint32_t e, uint32_t f, uint32_t g);

/**
 * @brief SHA‑256 majority function:
 *        Maj(a,b,c) = (a ∧ b) ⊕ (a ∧ c) ⊕ (b ∧ c)
 */
uint32_t maj(uint32_t a, uint32_t b, uint32_t c);

/**
 * @brief SHA‑256 big Sigma₀:
 *        Σ₀(a) = ROTR²(a) ⊕ ROTR¹³(a) ⊕ ROTR²²(a)
 */
uint32_t Sigma0(uint32_t a);

/**
 * @brief SHA‑256 big Sigma₁:
 *        Σ₁(e) = ROTR⁶(e) ⊕ ROTR¹¹(e) ⊕ ROTR²⁵(e)
 */
uint32_t Sigma1(uint32_t e);

/**
 * @brief Pads message according to SHA‑256 specification.
 *
 * Padding:
 *   • append 0x80  
 *   • append zeros until message ≡ 56 mod 64  
 *   • append 64‑bit big‑endian length  
 */
std::vector<uint8_t> pad(const std::vector<uint8_t>& input);

/**
 * @brief Parses a 512‑bit block (64 bytes) into 16 big‑endian 32‑bit words.
 */
std::vector<uint32_t> parse_block(const std::vector<uint8_t>& msg, size_t blockIndex);

/**
 * @brief Expands message schedule W[0..15] → W[0..63].
 */
void expand_w(std::vector<uint32_t>& W);

/**
 * @brief SHA‑256 compression function operating on a single block.
 */
void compress_block(std::vector<uint32_t>& H, const std::vector<uint32_t>& W);

/**
 * @brief Computes SHA‑256 hash of binary data.
 */
std::vector<uint8_t> sha256(const std::vector<uint8_t>& message);

/**
 * @brief Computes SHA‑256 hash of a string and returns hex string.
 */
std::string sha256(const std::string& message);

}
