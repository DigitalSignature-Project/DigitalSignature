#pragma once

#include <digisign/BigInt.h>

namespace digisign {

/**
 * @brief Computes n mod p using Barrett reduction optimized for small p.
 *
 * This function is used for primality testing and small‑prime sieving.
 * It assumes:
 *  - p < 2^32
 *  - R_mod_p = (2^64 mod p)
 *  - mu = floor(2^64 / p)
 *
 * @param n         Big integer to reduce
 * @param small_prime  modulus p (32‑bit)
 * @param R_mod_p   precomputed 2^64 mod p
 * @param mu        precomputed floor(2^64 / p)
 *
 * @return n mod p
 */
uint32_t small_mod_barrett(const BigInt& n, uint32_t small_prime, uint64_t R_mod_p, uint64_t mu);

/**
 * @brief Modular exponentiation using classic square‑and‑multiply.
 *
 * @param base  base value
 * @param exp   exponent
 * @param mod   modulus
 *
 * @return base^exp mod mod
 */
BigInt mod_pow(const BigInt& base, const BigInt& exp, const BigInt& mod);

/**
 * @brief Montgomery reduction: computes (a * R^{-1}) mod n.
 *
 * Assumes:
 *  - n is odd
 *  - n_inv = -n^{-1} mod 2^64
 *  - a < n * R
 *
 * @param a       input value in Montgomery domain
 * @param n       modulus
 * @param inverse_mod_n  n_inv = -n^{-1} mod 2^64
 *
 * @return reduced value
 */
BigInt montgomery_reduce(const BigInt& a, const BigInt& n, uint64_t inverse_mod_n);

/**
 * @brief Computes modular inverse of n modulo 2^k using Newton iteration.
 *
 * @param n  modulus (only limb[0] is used)
 * @param power_of_two  number of iterations (typically 6 for 64‑bit)
 *
 * @return n^{-1} mod 2^64
 */
uint64_t n_inv(const BigInt& n, int power_of_two);

/**
 * @brief Raw Montgomery exponentiation using sliding window method.
 *
 * @param base   base (normal domain)
 * @param exp    exponent
 * @param mod    modulus
 * @param window_bit_size  window size (w)
 * @param table_reference  preallocated table for odd powers
 * @param R2     R^2 mod n
 * @param inverse_mod_n  n_inv = -n^{-1} mod 2^64
 *
 * @return base^exp mod mod (Montgomery domain)
 */
BigInt montgomery_mod_pow_raw(BigInt base, BigInt exp, const BigInt& mod,
                              int window_bit_size, std::vector<BigInt>& table_reference,
                              BigInt& R2, uint64_t inverse_mod_n);

/**
 * @brief Full Montgomery exponentiation (raw + final reduction).
 */
BigInt montgomery_mod_pow(BigInt base, BigInt exp, const BigInt& mod,
                          int window_bit_size, std::vector<BigInt>& table_reference,
                          BigInt& R2, uint64_t inverse_mod_n);

/**
 * @brief Extended Euclidean algorithm helper (recursive).
 *
 * Computes gcd(a, b) and Bézout coefficients.
 * Internal function — handles sign tracking.
 */
BigInt extended_gcd_helper(const BigInt& a, const BigInt& b,
                           BigInt& x, BigInt& y, int& signx, int& signy);

/**
 * @brief Extended Euclidean algorithm.
 *
 * Computes:
 *      g = gcd(a, b)
 *      ax + by = g
 *
 * @param a input
 * @param b input
 * @param x output coefficient
 * @param y output coefficient
 *
 * @return gcd(a, b)
 */
BigInt extended_gcd(const BigInt& a, const BigInt& b, BigInt& x, BigInt& y);

/**
 * @brief Returns bit i of BigInt (little‑endian limbs).
 */
inline bool get_bit(const BigInt& x, int i) {
    return (x.limbs[i / 64] >> (i % 64)) & 1;
}

}
