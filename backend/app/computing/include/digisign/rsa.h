#pragma once

#include <digisign/BigInt.h>

namespace digisign {

/**
 * @brief Generates RSA keypair (public exponent e, private exponent d, modulus n).
 *
 * Steps:
 *   • generate two random primes p and q
 *   • compute n = p·q
 *   • compute φ(n) = (p−1)(q−1)
 *   • compute d = e⁻¹ mod φ(n) using extended GCD
 *
 * Public key  = (e, n)
 * Private key = (d, n)
 */
void RSA_generate_keys(BigInt& key_pub, BigInt& key_priv, BigInt& n, int bits);

/**
 * @brief Parallel version of RSA key generation using OpenMP.
 */
void RSA_generate_keys_parallel(BigInt& key_pub, BigInt& key_priv, BigInt& n,
                                int bits, int max_threads);

/**
 * @brief RSA encryption: c = mᵉ mod n
 */
BigInt encrypt(const BigInt& message, const BigInt& pub_key, const BigInt& n);

/**
 * @brief RSA decryption: m = cᵈ mod n
 */
BigInt decrypt(const BigInt& encrypted_message, const BigInt& priv_key, const BigInt& n);

}
