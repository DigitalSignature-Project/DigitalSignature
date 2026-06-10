#pragma once
#include <digisign/BigInt.h>
#include <functional>
#include <string>

namespace digisign {

/**
 * @brief Public key for the ElGamal signature scheme.
 *
 * The key consists of:
 *   - p : a large prime modulus
 *   - q : a prime divisor of p−1 (order of subgroup)
 *   - g : generator of subgroup of order q
 *   - y : public key value y = g^x mod p
 *
 * The private key is the integer x ∈ [1, q−1].
 */
struct ElGamalPublicKey {

    BigInt p; ///< Prime modulus
    BigInt q; ///< Order of subgroup
    BigInt g; ///< Generator of subgroup
    BigInt y; ///< Public key value y = g^x mod p

    /**
     * @brief Construct a fully initialized public key.
     */
    ElGamalPublicKey(const BigInt& p,
                     const BigInt& q,
                     const BigInt& g,
                     const BigInt& y);

    /**
     * @brief Construct an empty (uninitialized) public key.
     */
    ElGamalPublicKey();
};

/**
 * @brief ElGamal signature consisting of two integers (r, s).
 *
 * Signature equations:
 *   r = (g^k mod p) mod q
 *   s = k^{-1}(H(m) + x r) mod q
 *
 * Both r and s lie in the range [1, q−1].
 */
struct ElGamalSignature {

    BigInt r; ///< First signature component
    BigInt s; ///< Second signature component

    /**
     * @brief Construct a signature from components r and s.
     */
    ElGamalSignature(const BigInt& r, const BigInt& s);

    /**
     * @brief Construct an empty signature.
     */
    ElGamalSignature();
};

/**
 * @brief Generate a generator g of the subgroup of order q in (Z/pZ)*.
 *
 * Standard construction:
 *   g = h^{(p−1)/q} mod p
 * for random h ∈ [2, p−2], rejecting g = 1.
 *
 * @param p Prime modulus.
 * @param q Prime divisor of p−1.
 * @param bits Bit length of p (used to choose window size).
 * @return A generator g of the subgroup of order q.
 */
BigInt generate_g(const BigInt& p, const BigInt& q, int bits);

/**
 * @brief Generate an ElGamal keypair (sequential version).
 *
 * Steps:
 *   1. Generate prime q.
 *   2. Generate p = kq + 1 with p prime.
 *   3. Compute generator g of subgroup of order q.
 *   4. Choose private key x ∈ [1, q−1].
 *   5. Compute public key y = g^x mod p.
 *
 * @param key_pub Output: public key (p, q, g, y).
 * @param key_priv Output: private key x.
 * @param bits_p Bit length of p.
 * @param bits_q Bit length of q.
 *
 * @throws std::runtime_error if bits_p < bits_q.
 */
void ElGamal_generate_keys(ElGamalPublicKey& key_pub,
                           BigInt& key_priv,
                           const int bits_p,
                           const int bits_q);

/**
 * @brief Parallel ElGamal key generation using OpenMP.
 *
 * Uses multiple threads to:
 *   - generate q in parallel
 *   - search for p = kq + 1 in parallel
 *
 * @param key_pub Output: public key.
 * @param key_priv Output: private key.
 * @param bits_p Bit length of p.
 * @param bits_q Bit length of q.
 * @param threads Number of threads to use.
 *
 * @throws std::runtime_error if bits_p < bits_q.
 */
void ElGamal_generate_keys_parallel(ElGamalPublicKey& key_pub,
                                    BigInt& key_priv,
                                    const int bits_p,
                                    const int bits_q,
                                    const int threads);

/**
 * @brief Parallel search for prime p = kq + 1.
 *
 * Each thread independently samples k and tests p for primality.
 * First thread to find valid p stores it atomically.
 *
 * @param bits_p Bit length of p.
 * @param bits_q Bit length of q.
 * @param q Prime divisor of p−1.
 * @param threads Number of threads.
 * @return A prime p = kq + 1 of the requested size.
 */
BigInt generate_p_parallel(const int bits_p,
                           const int bits_q,
                           const BigInt& q,
                           const int threads);

/**
 * @brief Sign a message using the ElGamal signature scheme.
 *
 * Signature equations:
 *   r = (g^k mod p) mod q
 *   s = k^{-1}(H(m) + x r) mod q
 *
 * Hash is reduced to q_bits (DSA-style hash_to_int).
 *
 * @param message Message to sign.
 * @param key_pub Public key (p, q, g, y).
 * @param key_priv Private key x.
 * @param hash_function Hash function H: bytes → bytes.
 *
 * @return ElGamal signature (r, s).
 */
ElGamalSignature elgamal_sign(const std::string& message,
                              const ElGamalPublicKey& key_pub,
                              const BigInt& key_priv,
                              std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

/**
 * @brief Verify an ElGamal signature.
 *
 * Verification equations:
 *   w  = s^{-1} mod q
 *   u1 = H(m) w mod q
 *   u2 = r w mod q
 *   v  = (g^{u1} y^{u2} mod p) mod q
 *
 * Signature is valid iff v == r.
 *
 * @param message Message to verify.
 * @param key_pub Public key.
 * @param signature Signature (r, s).
 * @param hash_function Hash function H.
 *
 * @return true if signature is valid, false otherwise.
 */
bool elgamal_verify(const std::string& message,
                    const ElGamalPublicKey& key_pub,
                    const ElGamalSignature& signature,
                    std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

/**
 * @brief Verify a hex-encoded DER signature.
 *
 * @param message Message to verify.
 * @param key_pub Public key.
 * @param hex_signature DER-encoded signature in hex.
 * @param hash_function Hash function H.
 *
 * @return true if signature is valid, false otherwise.
 */
bool elgamal_verify(const std::string& message,
                    const ElGamalPublicKey& key_pub,
                    const std::string& hex_signature,
                    std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

/**
 * @brief Decode DER-encoded signature.
 *
 * Format:
 *   SEQUENCE {
 *     INTEGER r
 *     INTEGER s
 *   }
 *
 * @param sig Raw DER bytes.
 * @return Parsed ElGamal signature.
 *
 * @throws std::runtime_error on invalid DER.
 */
ElGamalSignature DER_decode_signature(const std::vector<uint8_t>& sig);

/**
 * @brief Decode DER signature from hex string.
 *
 * @param hex_sig Hex-encoded DER signature.
 * @return Parsed ElGamal signature.
 */
ElGamalSignature DER_decode_signature(const std::string& hex_sig);

/**
 * @brief Encode signature into DER format.
 *
 * Format:
 *   SEQUENCE {
 *     INTEGER r
 *     INTEGER s
 *   }
 *
 * @param sig Signature to encode.
 * @return DER-encoded signature bytes.
 */
std::vector<uint8_t> DER_encode_signature(const ElGamalSignature& sig);

/**
 * @brief Encode signature into hex-encoded DER.
 *
 * @param sig Signature to encode.
 * @return Hex string containing DER encoding.
 */
std::string DER_encode_signature_hex(const ElGamalSignature& sig);

}