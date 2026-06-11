#pragma once

#include <digisign/BigInt.h>
#include <digisign/rng.h>

namespace digisign {

/**
 * @brief Miller–Rabin probabilistic primality test.
 *
 * Performs the standard Miller–Rabin test using Montgomery exponentiation.
 *
 * @param n        Number to test (must be ≥ 2)
 * @param rounds   Number of random bases to test
 * @param generator Random generator for bases
 * @param w        Window size for Montgomery exponentiation
 * @param table    Preallocated table for odd powers (size = 2^(w−1))
 *
 * @return true if n is probably prime, false if composite
 */
bool miller_rabin(const BigInt& n, int rounds, RandomGenerator& generator,
                  int w, std::vector<BigInt>& table);

/**
 * @brief Fast small‑prime sieve using Barrett reduction.
 *
 * Tests divisibility by all primes < 1000.
 *
 * @return false if n is divisible by any small prime, true otherwise
 */
bool small_prime_test(const BigInt& n);

/**
 * @brief Generates a random probable prime of given bit length.
 *
 * Uses:
 *  • wheel‑based prime candidates
 *  • small prime sieve
 *  • Miller–Rabin test
 */
BigInt generate_prime(size_t bits, int miller_rabin_rounds, RandomGenerator& generator);

/**
 * @brief Parallel version of generate_prime using OpenMP.
 *
 * Spawns multiple threads, each searching independently.
 */
BigInt generate_prime_parallel_omp(size_t bits, int miller_rabin_rounds, int max_threads);

}
