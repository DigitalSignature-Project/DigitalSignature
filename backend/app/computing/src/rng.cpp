#include <random>
#include <openssl/rand.h>
#include <digisign/BigInt.h>
#include <digisign/rng.h>

namespace digisign {

RandomGenerator::RandomGenerator()
{
    init_wheel();
}

uint64_t RandomGenerator::next64() {
    uint64_t value;

    for (int i = 0; i < 10; i++) {
        if (RAND_bytes(reinterpret_cast<unsigned char*>(&value), sizeof(value)) == 1) {
            return value;
        }
    }

    std::random_device rd;
    return (uint64_t(rd()) << 32) | rd();
}

void RandomGenerator::init_wheel() {
    for (uint64_t r = 1; r < W; r++) {
        if (r % 2 != 0 && r % 3 != 0 && r % 5 != 0 && r % 7 != 0 && r % 11 != 0 && r % 13 != 0 && r % 17 != 0)
            wheel_remainders.push_back(r);
    }
}

void RandomGenerator::generate_random_bigint(BigInt& n)
{
    for (size_t i = 0; i < n.used; i++)
        n.limbs[i] = next64();
}


void RandomGenerator::generate_random_bigint(BigInt& n, size_t bits)
{
    size_t limb_count = (bits + 63) / 64;

    if (n.limbs.size() < limb_count)
        n.limbs.resize(limb_count);

    n.used = limb_count;

    for (size_t i = 0; i < n.used; i++)
        n.limbs[i] = next64();
}

void RandomGenerator::generate_random_bigint_prime_candidate(BigInt& n)
{
    for (size_t i = 0; i < n.used; i++)
        n.limbs[i] = next64();

    n.limbs[n.used - 1] |= (1ULL << 63);

    uint64_t rem = wheel_remainders[next64() % wheel_remainders.size()];

    uint64_t low = n.limbs[0];
    low -= low % W;
    low += rem;
    n.limbs[0] = low;

    n.limbs[0] |= 1ULL;
}

void RandomGenerator::generate_random_bigint_prime_candidate(BigInt& n, size_t bits) {
    generate_random_bigint(n, bits);

    size_t limb_count = (bits + 63) / 64;

    n.limbs[limb_count - 1] |= (1ULL << ((bits - 1) % 64));

    uint64_t rem = wheel_remainders[next64() % wheel_remainders.size()];

    uint64_t low = n.limbs[0];
    low -= low % W;
    low += rem;
    n.limbs[0] = low;

    n.limbs[0] |= 1;
}

BigInt RandomGenerator::random_range(const BigInt& min,
    const BigInt& max)
{
    BigInt r(max.used * 64);

    do
    {
        for (size_t i = 0; i < r.used; i++)
            r.limbs[i] = next64();

        r.normalize();

    } while (r < min || r > max);

    return r;
}

}
