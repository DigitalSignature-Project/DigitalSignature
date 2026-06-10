#include <digisign/rsa.h>
#include <digisign/rng.h>
#include <digisign/prime.h>
#include <digisign/mod_arith.h>

namespace digisign {

// ------------------------------------------------------------
// RSA key generation
// ------------------------------------------------------------
void RSA_generate_keys(BigInt& key_pub, BigInt& key_priv, BigInt& n, int bits) {
    // RSA modulus n = p·q, each prime roughly bits/2
    bits /= 2;
    bits = std::max(64, bits);

    // Standard public exponent e = 65537
    BigInt e(64);
    e.limbs[0] = 65537;

    RandomGenerator rng;

    // Generate primes p1, p2
    BigInt p1 = generate_prime(bits, 40, rng);
    BigInt p2 = generate_prime(bits, 40, rng);

    // Ensure gcd(e, p−1) = 1
    while (((p1 - 1) % e).isZero()) {
        p1 = generate_prime(bits, 40, rng);
    }
    while (((p2 - 1) % e).isZero() || p1 == p2) {
        p2 = generate_prime(bits, 40, rng);
    }

    // n = p·q
    n = p1 * p2;

    // φ(n) = (p−1)(q−1)
    BigInt phi_n = (p1 - 1) * (p2 - 1);

    // Compute d = e⁻¹ mod φ(n)
    BigInt x, y;
    extended_gcd(e, phi_n, x, y);

    // Normalize d into [0, φ(n))
    if (!(x < phi_n)) {
        x = x % phi_n;
    }

    key_pub = e;
    key_priv = x;
}

// ------------------------------------------------------------
// Parallel RSA key generation
// ------------------------------------------------------------
void RSA_generate_keys_parallel(BigInt& key_pub, BigInt& key_priv, BigInt& n,
                                int bits, int threads) {
    bits /= 2;
    bits = std::max(64, bits);

    BigInt e(64);
    e.limbs[0] = 65537;

    // Parallel prime generation
    BigInt p1 = generate_prime_parallel_omp(bits, 40, threads);
    BigInt p2 = generate_prime_parallel_omp(bits, 40, threads);

    while (((p1 - 1) % e).isZero()) {
        p1 = generate_prime_parallel_omp(bits, 40, threads);
    }
    while (((p2 - 1) % e).isZero() || p1 == p2) {
        p2 = generate_prime_parallel_omp(bits, 40, threads);
    }

    n = p1 * p2;
    BigInt phi_n = (p1 - 1) * (p2 - 1);

    BigInt x, y;
    extended_gcd(e, phi_n, x, y);

    if (!(x < phi_n)) {
        x = x % phi_n;
    }

    key_pub  = e;
    key_priv = x;
}

// ------------------------------------------------------------
// RSA encryption: c = mᵉ mod n
// ------------------------------------------------------------
BigInt encrypt(const BigInt& message, const BigInt& pub_key, const BigInt& n) {
    BigInt encrypted_message = message;

    size_t n_limbs = n.used;

    // Window size for Montgomery exponentiation
    int w = 2;

    int table_size = 1 << (w - 1);
    std::vector<BigInt> table(table_size);

    // Montgomery constants
    BigInt R((n_limbs + 1) * 64);
    R.limbs[n_limbs] = 1;
    R.used = n_limbs + 1;

    BigInt R2 = (R * R) % n;
    uint64_t n0_inv = n_inv(n, 6);

    // Perform modular exponentiation using Montgomery arithmetic
    return montgomery_mod_pow(encrypted_message, pub_key, n, w, table, R2, n0_inv);
}

// ------------------------------------------------------------
// RSA decryption: m = cᵈ mod n
// ------------------------------------------------------------
BigInt decrypt(const BigInt& encrypted_message, const BigInt& priv_key, const BigInt& n) {
    BigInt message = encrypted_message;

    size_t n_limbs = n.used;

    int w = 2;

    int table_size = 1 << (w - 1);
    std::vector<BigInt> table(table_size);

    BigInt R((n_limbs + 1) * 64);
    R.limbs[n_limbs] = 1;
    R.used = n_limbs + 1;

    BigInt R2 = (R * R) % n;
    uint64_t n0_inv = n_inv(n, 6);

    return montgomery_mod_pow(message, priv_key, n, w, table, R2, n0_inv);
}

}
