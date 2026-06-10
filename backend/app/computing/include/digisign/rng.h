#pragma once

#include <digisign/BigInt.h>

namespace digisign {

/**
 * @brief Cryptographically secure random number generator.
 *
 * This generator provides:
 *  • 64‑bit random values (next64)
 *  • random BigInt generation
 *  • prime candidate generation using wheel factorization
 *  • uniform random values in a BigInt range
 *
 * It uses OpenSSL RAND_bytes as the primary entropy source,
 * with std::random_device as a fallback.
 */
class RandomGenerator {

private:
    /**
     * @brief Precomputed residues for wheel factorization.
     *
     * Wheel modulus:
     *      W = 2·3·5·7·11·13·17
     *
     * wheel_remainders contains all r < W such that gcd(r, W) = 1.
     * This allows generating numbers that automatically skip
     * small prime factors, improving primality testing efficiency.
     */
    std::vector<uint64_t> wheel_remainders;

    /// Product of first primes used for wheel factorization.
    uint64_t W = 2 * 3 * 5 * 7 * 11 * 13 * 17;

public:
    /** @brief Initializes wheel residues. */
    RandomGenerator();

    /**
     * @brief Returns a cryptographically secure 64‑bit random value.
     *
     * Attempts OpenSSL RAND_bytes up to 10 times.
     * If all attempts fail, falls back to std::random_device.
     */
    uint64_t next64();

    /**
     * @brief Initializes wheel_remainders with residues coprime to W.
     */
    void init_wheel();

    /**
     * @brief Fills an existing BigInt with random 64‑bit limbs.
     */
    void generate_random_bigint(BigInt& n);

    /**
     * @brief Generates a random BigInt with a specific bit length.
     */
    void generate_random_bigint(BigInt& n, size_t bits);

    /**
     * @brief Generates a random BigInt suitable as a prime candidate.
     *
     * Ensures:
     *  • highest bit is set (correct bit length)
     *  • number is odd
     *  • number ≡ r (mod W) for r ∈ wheel_remainders
     */
    void generate_random_bigint_prime_candidate(BigInt& n);

    /**
     * @brief Same as above, but with explicit bit length.
     */
    void generate_random_bigint_prime_candidate(BigInt& n, size_t bits);

    /**
     * @brief Returns a random BigInt in the inclusive range [min, max].
     *
     * Uses rejection sampling.
     */
    BigInt random_range(const BigInt& min, const BigInt& max);
};

}
