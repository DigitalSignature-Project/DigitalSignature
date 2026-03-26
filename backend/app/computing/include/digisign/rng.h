#pragma once

#include <digisign/BigInt.h>

namespace digisign {

class RandomGenerator {

private:
    std::vector<uint64_t> wheel_remainders;

    uint64_t W = 2 * 3 * 5 * 7 * 11 * 13 * 17;

public:
    RandomGenerator();

    uint64_t next64();

    void init_wheel();

    void generate_random_bigint(BigInt& n);

    void generate_random_bigint(BigInt& n, size_t bits);

    void generate_random_bigint_prime_candidate(BigInt& n);

    void generate_random_bigint_prime_candidate(BigInt& n, size_t bits);

    BigInt random_range(const BigInt& min, const BigInt& max);
};

}