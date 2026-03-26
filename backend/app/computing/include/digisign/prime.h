#pragma once

#include <digisign/BigInt.h>
#include <digisign/rng.h>

namespace digisign {

	bool miller_rabin(const BigInt& n, int rounds, RandomGenerator& generator, int w, std::vector<BigInt>& table);

	bool small_prime_test(const BigInt& n);

	BigInt generate_prime(size_t bits, int miller_rabin_rounds, RandomGenerator& generator);

	BigInt generate_prime_parallel_omp(size_t bits, int miller_rabin_rounds, int max_threads);

}