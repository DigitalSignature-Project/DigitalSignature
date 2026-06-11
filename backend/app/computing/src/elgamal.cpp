#include <digisign/elgamal.h>
#include <digisign/rng.h>
#include <digisign/prime.h>
#include <digisign/mod_arith.h>
#include <iostream>
#include <digisign/format.h>
#include <vector>

namespace digisign {

/**
 * Public key structure:
 *   p — large prime modulus
 *   q — prime divisor of p−1 (order of subgroup)
 *   g — generator of subgroup of order q
 *   y = g^x mod p — public component
 */
ElGamalPublicKey::ElGamalPublicKey(const BigInt& p, const BigInt& q,
                                   const BigInt& g, const BigInt& y)
    : p(p), q(q), g(g), y(y) {}

ElGamalPublicKey::ElGamalPublicKey()
    : p(BigInt()), q(BigInt()), g(BigInt()), y(BigInt()) {}

/**
 * Signature structure:
 *   r = (g^k mod p) mod q
 *   s = k^{-1}(H(m) + x r) mod q
 */
ElGamalSignature::ElGamalSignature(const BigInt& r, const BigInt& s)
    : r(r), s(s) {}

ElGamalSignature::ElGamalSignature()
    : r(BigInt()), s(BigInt()) {}

/**
 * Generate generator g of subgroup of order q in (Z/pZ)*.
 *
 * Standard construction:
 *   g = h^{(p−1)/q} mod p
 * for random h ∈ [2, p−2], rejecting g = 1.
 *
 * Uses Montgomery exponentiation for performance and side‑channel resistance.
 */
BigInt generate_g(const BigInt& p, const BigInt& q, int bits) {
    RandomGenerator rng;
    BigInt two = BigInt::two();
    BigInt one = BigInt::one();

    // Window size for Montgomery exponentiation
    int w = (bits < 256 ? 4 :
            bits < 1024 ? 5 :
            bits < 2048 ? 6 : 7);

    std::vector<BigInt> table(1 << (w - 1));

    // Montgomery constants
    size_t p_limbs = p.used;
    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % p;
    uint64_t p0_inv = n_inv(p, 6);

    BigInt exp = (p - one) / q;  // exponent for subgroup generator
    BigInt h, g;

    while (true) {
        h = rng.random_range(two, p - two);

        g = montgomery_mod_pow(h, exp, p, w, table, R2, p0_inv);

        if (g != one)
            return g;
    }
}

/**
 * Generate ElGamal keypair (sequential version).
 *
 * Steps:
 *   1. Generate prime q.
 *   2. Generate p = kq + 1 of desired size, with p prime.
 *   3. Generate generator g of subgroup of order q.
 *   4. Choose private key x ∈ [1, q−1].
 *   5. Compute y = g^x mod p.
 */
void ElGamal_generate_keys(ElGamalPublicKey& key_pub, BigInt& key_priv,
                           const int bits_p, const int bits_q) {

    if (bits_p < bits_q)
        throw std::runtime_error("p must be bigger than q");

    RandomGenerator rng;
    BigInt q = generate_prime(bits_q, 40, rng);

    BigInt p, k;
    BigInt one = BigInt::one();

    int w = (bits_p < 256 ? 4 :
            bits_p < 1024 ? 5 :
            bits_p < 2048 ? 6 : 7);

    std::vector<BigInt> table(1 << (w - 1));

    // Generate p = kq + 1 with p prime
    while (true) {
        k = rng.random_range(BigInt::one() << (bits_p - bits_q - 1),
                             (BigInt::one() << (bits_p - bits_q)) - 1);

        p = k * q + one;

        if (p.bit_length() != bits_p)
            continue;

        if (!small_prime_test(p))
            continue;

        if (miller_rabin(p, 40, rng, w, table))
            break;
    }

    // Generator of subgroup
    BigInt g = generate_g(p, q, bits_p);

    // Private key x ∈ [1, q−1]
    key_priv = rng.random_range(one, q - one);

    // Montgomery constants for computing y = g^x mod p
    size_t p_limbs = p.used;
    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % p;
    uint64_t p0_inv = n_inv(p, 6);

    key_pub.y = montgomery_mod_pow(g, key_priv, p, w, table, R2, p0_inv);

    key_pub.p = p;
    key_pub.q = q;
    key_pub.g = g;
}

/**
 * Parallel generation of p = kq + 1 using OpenMP.
 *
 * Multiple threads independently search for k such that p is prime.
 * First thread to find valid p stores it atomically.
 */
BigInt generate_p_parallel(const int bits_p, const int bits_q,
                           BigInt& q, const int threads) {

    BigInt p;
    BigInt one = BigInt::one();
    std::atomic<bool> found(false);

    int w = (bits_p < 256 ? 4 :
            bits_p < 1024 ? 5 :
            bits_p < 2048 ? 6 : 7);

#pragma omp parallel for num_threads(threads)
    for (int i = 0; i < threads; i++) {
        RandomGenerator rng;
        BigInt k, p_local;

        std::vector<BigInt> table_local(1 << (w - 1));

        while (true) {
            if (found.load(std::memory_order_relaxed))
                break;

            k = rng.random_range(BigInt::one() << (bits_p - bits_q - 1),
                                 (BigInt::one() << (bits_p - bits_q)) - 1);

            p_local = k * q + one;

            if (p_local.bit_length() != bits_p)
                continue;

            if (!small_prime_test(p_local))
                continue;

            if (miller_rabin(p_local, 40, rng, w, table_local)) {
#pragma omp critical
                if (!found.load(std::memory_order_relaxed)) {
                    p = p_local;
                    found.store(true, std::memory_order_relaxed);
                }
                break;
            }
        }
    }

    return p;
}

/**
 * Parallel key generation:
 *   - q generated in parallel
 *   - p generated in parallel
 *   - g computed sequentially
 *   - y = g^x mod p computed via Montgomery exponentiation
 */
void ElGamal_generate_keys_parallel(ElGamalPublicKey& key_pub, BigInt& key_priv,
                                    const int bits_p, const int bits_q,
                                    const int threads) {

    if (bits_p < bits_q)
        throw std::runtime_error("p must be bigger than q");

    RandomGenerator rng;

    BigInt q = generate_prime_parallel_omp(bits_q, 40, threads);
    BigInt p = generate_p_parallel(bits_p, bits_q, q, threads);

    BigInt g = generate_g(p, q, bits_p);

    BigInt one = BigInt::one();
    key_priv = rng.random_range(one, q - one);

    int w = (bits_p < 256 ? 4 :
            bits_p < 1024 ? 5 :
            bits_p < 2048 ? 6 : 7);

    std::vector<BigInt> table(1 << (w - 1));

    size_t p_limbs = p.used;
    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % p;
    uint64_t p0_inv = n_inv(p, 6);

    key_pub.y = montgomery_mod_pow(g, key_priv, p, w, table, R2, p0_inv);

    key_pub.p = p;
    key_pub.q = q;
    key_pub.g = g;
}


/**
 * Sign a message using ElGamal signature scheme.
 *
 * Signature equations:
 *   r = (g^k mod p) mod q
 *   s = k^{-1}(H(m) + x*r) mod q
 *
 * Steps:
 *   1. Hash message and reduce hash to integer < q.
 *   2. Sample ephemeral k ∈ [1, q−1].
 *   3. Compute r = (g^k mod p) mod q.
 *   4. Compute s = k^{-1}(H(m) + x*r) mod q.
 *
 * Security notes:
 *   - k must be uniformly random and never reused.
 *   - Hash is truncated to q_bits (DSA-style hash_to_int).
 *   - Montgomery exponentiation protects against timing attacks.
 */
ElGamalSignature elgamal_sign(const std::string& message,
                              const ElGamalPublicKey& key_pub,
                              const BigInt& key_priv,
                              std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function)
{
    // Convert message to bytes and hash it
    std::vector<uint8_t> messageBytes(message.begin(), message.end());
    std::vector<uint8_t> hashedMessage = hash_function(messageBytes);

    // Reduce hash to integer < q (DSA-style hash_to_int)
    size_t q_bits = key_pub.q.bit_length();
    size_t q_bytes = (q_bits + 7) / 8;

    if (hashedMessage.size() > q_bytes)
        hashedMessage.resize(q_bytes);

    int extra_bits = (q_bytes * 8) - q_bits;
    if (extra_bits > 0)
        hashedMessage[0] &= (0xFF >> extra_bits);

    BigInt hashedMessageBI = BigInt::vectoruint8(hashedMessage) % key_pub.q;

    RandomGenerator rng;
    BigInt k;

    // Montgomery exponentiation parameters
    size_t p_limbs = key_pub.p.used;
    int bits = p_limbs * 64;

    int w = (bits < 256 ? 4 :
            bits < 1024 ? 5 :
            bits < 2048 ? 6 : 7);

    std::vector<BigInt> table(1 << (w - 1));

    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % key_pub.p;
    uint64_t p0_inv = n_inv(key_pub.p, 6);

    BigInt r;

    // Sample k until r != 0
    do {
        k = rng.random_range(BigInt::one(), key_pub.q - BigInt::one());
        r = montgomery_mod_pow(key_pub.g, k, key_pub.p, w, table, R2, p0_inv) % key_pub.q;
    } while (r.isZero());

    // Compute k^{-1} mod q
    BigInt k_inv, tmp;
    extended_gcd(k, key_pub.q, k_inv, tmp);

    if (!(k_inv < key_pub.q))
        k_inv = k_inv % key_pub.q;

    // s = k^{-1}(H(m) + x r) mod q
    BigInt s = (k_inv * (hashedMessageBI + key_priv * r)) % key_pub.q;

    return ElGamalSignature(r, s);
}

/**
 * Verify ElGamal signature.
 *
 * Verification equations:
 *   w = s^{-1} mod q
 *   u1 = H(m) w mod q
 *   u2 = r w mod q
 *   v = (g^{u1} y^{u2} mod p) mod q
 * Signature valid iff v == r.
 *
 * Security notes:
 *   - Hash reduction must match signing side exactly.
 *   - All exponentiations use Montgomery ladder for timing safety.
 */
bool elgamal_verify(const std::string& message,
                    const ElGamalPublicKey& key_pub,
                    const ElGamalSignature& signature,
                    std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function)
{
    // Reject invalid ranges
    if (signature.r.isZero() || !(signature.r < key_pub.q)) return false;
    if (signature.s.isZero() || !(signature.s < key_pub.q)) return false;

    // Compute w = s^{-1} mod q
    BigInt w, tmp;
    extended_gcd(signature.s, key_pub.q, w, tmp);

    if (!(w < key_pub.q))
        w = w % key_pub.q;

    // Hash message and reduce to integer < q
    std::vector<uint8_t> messageBytes(message.begin(), message.end());
    std::vector<uint8_t> hashedMessage = hash_function(messageBytes);

    size_t q_bits = key_pub.q.bit_length();
    size_t q_bytes = (q_bits + 7) / 8;

    if (hashedMessage.size() > q_bytes)
        hashedMessage.resize(q_bytes);

    int extra_bits = (q_bytes * 8) - q_bits;
    if (extra_bits > 0)
        hashedMessage[0] &= (0xFF >> extra_bits);

    BigInt hashedMessageBI = BigInt::vectoruint8(hashedMessage) % key_pub.q;

    // Compute u1, u2
    BigInt u1 = (hashedMessageBI * w) % key_pub.q;
    BigInt u2 = (signature.r * w) % key_pub.q;

    // Montgomery exponentiation parameters
    size_t p_limbs = key_pub.p.used;
    int bits = p_limbs * 64;

    int window = (bits < 256 ? 4 :
                 bits < 1024 ? 5 :
                 bits < 2048 ? 6 : 7);

    std::vector<BigInt> table(1 << (window - 1));

    BigInt R((p_limbs + 1) * 64);
    R.limbs[p_limbs] = 1;
    R.used = p_limbs + 1;

    BigInt R2 = (R * R) % key_pub.p;
    uint64_t p0_inv = n_inv(key_pub.p, 6);

    // Compute g^{u1} and y^{u2}
    BigInt gu1 = montgomery_mod_pow(key_pub.g, u1, key_pub.p, window, table, R2, p0_inv);
    BigInt yu2 = montgomery_mod_pow(key_pub.y, u2, key_pub.p, window, table, R2, p0_inv);

    // v = (g^{u1} y^{u2} mod p) mod q
    BigInt v = ((gu1 * yu2) % key_pub.p) % key_pub.q;

    return (v == signature.r);
}

/**
 * Verify signature from hex-encoded DER.
 */
bool elgamal_verify(const std::string& message,
                    const ElGamalPublicKey& key_pub,
                    const std::string& hex_signature,
                    std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function)
{
    return elgamal_verify(message, key_pub,
                          DER_decode_signature(hex_signature),
                          hash_function);
}

/**
 * Decode DER-encoded signature:
 *
 * SEQUENCE {
 *   INTEGER r
 *   INTEGER s
 * }
 *
 * Uses standard ASN.1 DER INTEGER rules:
 *   - Leading 0x00 allowed only to avoid negative interpretation.
 *   - Length must match exactly.
 */
ElGamalSignature DER_decode_signature(const std::vector<uint8_t>& sig) {
    size_t offset = 0;

    if (sig[offset++] != 0x30)
        throw std::runtime_error("Invalid signature: expected SEQUENCE");

    size_t seqLen = DER_decode_length(sig, offset);
    size_t seqEnd = offset + seqLen;

    if (seqEnd > sig.size())
        throw std::runtime_error("Invalid signature: length overflow");

    std::vector<uint8_t> r = DER_decode_integer(sig, offset);
    std::vector<uint8_t> s = DER_decode_integer(sig, offset);

    if (offset != seqEnd)
        throw std::runtime_error("Invalid signature: trailing bytes");

    return ElGamalSignature(BigInt::vectoruint8(r),
                            BigInt::vectoruint8(s));
}

ElGamalSignature DER_decode_signature(const std::string& hex_sig) {
    return DER_decode_signature(hex_to_bytes(hex_sig));
}

/**
 * Encode signature into DER:
 *
 * SEQUENCE {
 *   INTEGER r
 *   INTEGER s
 * }
 *
 * Output is compatible with OpenSSL, Botan, BouncyCastle.
 */
std::vector<uint8_t> DER_encode_signature(const ElGamalSignature& sig) {
    std::vector<uint8_t> r_bytes = sig.r.to_vectoruint8();
    std::vector<uint8_t> s_bytes = sig.s.to_vectoruint8();

    std::vector<uint8_t> r_enc = DER_encode_integer(r_bytes);
    std::vector<uint8_t> s_enc = DER_encode_integer(s_bytes);

    std::vector<uint8_t> body;
    body.insert(body.end(), r_enc.begin(), r_enc.end());
    body.insert(body.end(), s_enc.begin(), s_enc.end());

    std::vector<uint8_t> out;
    out.push_back(0x30); // SEQUENCE

    std::vector<uint8_t> len = DER_encode_length(body.size());
    out.insert(out.end(), len.begin(), len.end());
    out.insert(out.end(), body.begin(), body.end());

    return out;
}

std::string DER_encode_signature_hex(const ElGamalSignature& sig) {
    return bytes_to_hex(DER_encode_signature(sig));
}

}
