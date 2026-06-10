#include <digisign/mod_arith.h>
#include <stdexcept>

namespace digisign {

uint32_t small_mod_barrett(const BigInt& n, uint32_t p, uint64_t R_mod_p, uint64_t mu) {
    // Barrett reduction specialized for 32‑bit modulus.
    // Processes limbs from most significant to least significant.
    //
    // Uses recurrence:
    //   result = (result * 2^64 + limb) mod p
    //
    // Multiplications by 2^64 mod p and floor(2^64 / p) are precomputed.

    uint64_t result = 0;

    for (int i = n.used - 1; i >= 0; i--) {
        uint64_t limb = n.limbs[i];

        // Multiply current result by 2^64 mod p
        uint64_t hi1, lo1;
        lo1 = _umul128(result, R_mod_p, &hi1);

        // Add next limb
        uint64_t lo = lo1 + limb;
        uint64_t hi = hi1 + (lo < limb ? 1 : 0);

        // Approximate quotient using Barrett constant mu
        uint64_t hi2, lo2;
        lo2 = _umul128(lo, mu, &hi2);
        uint64_t q = hi * mu + hi2;

        // Subtract q * p
        lo2 = _umul128(q, p, &hi2);

        uint64_t r = lo - lo2;
        if (r > lo) hi2++;

        r = r + hi - hi2;

        // Final correction
        if (r >= p) r -= p;
        if (r >= p) r -= p;

        result = r;
    }

    return (uint32_t)result;
}

BigInt mod_pow(const BigInt& base, const BigInt& exp, const BigInt& mod) {
    // Classic binary exponentiation (square‑and‑multiply).
    BigInt result = BigInt::one();
    BigInt b = base % mod;
    BigInt e = exp;

    while (!e.isZero()) {
        if ((e.limbs[0] & 1) != 0)
            result = (result * b) % mod;

        e = e >> 1;
        b = (b * b) % mod;
    }
    return result;
}

uint64_t n_inv(const BigInt& n, int k) {
    // Computes modular inverse of n modulo 2^64 using Newton iteration:
    //   x_{i+1} = x_i * (2 - n * x_i)
    //
    // After k iterations, x ≈ n^{-1} mod 2^64.

    uint64_t n0 = n.limbs[0];
    uint64_t x = 1;

    for (int i = 0; i < k; i++) {
        x = x * (2 - n0 * x);
    }

    // Return -x mod 2^64 (Montgomery requires n_inv = -n^{-1})
    return (~x) + 1;
}

BigInt montgomery_reduce(const BigInt& t, const BigInt& n, uint64_t n_inv) {
    // Montgomery reduction:
    //   Computes (t + m*n) / R  where m = (t * n_inv) mod R
    //
    // Assumes:
    //   - R = 2^(64*k)
    //   - t < n * R
    //   - n is odd

    if (t.used > 2 * n.used) {
        throw std::runtime_error("t mustn't have more limbs than 2x n's limbs");
    }

    size_t k = n.used;

    // Copy t into res and ensure enough space
    BigInt res = t;
    res.limbs.resize(2 * k);
    res.used = 2 * k;

    // Main Montgomery loop
    for (size_t i = 0; i < k; i++) {
        uint64_t m_i = res.limbs[i] * n_inv;

        uint64_t carry = 0;
        for (size_t j = 0; j < k; j++) {
            uint64_t hi;
            uint64_t lo = _umul128(m_i, n.limbs[j], &hi);

            uint64_t sum = res.limbs[i + j] + lo;
            uint64_t carry1 = (sum < lo);

            sum += carry;
            uint64_t carry2 = (sum < carry);

            res.limbs[i + j] = sum;
            carry = hi + carry1 + carry2;
        }

        // Propagate carry
        size_t pos = i + k;
        while (carry != 0) {
            if (pos >= res.limbs.size()) {
                res.limbs.push_back(0);
                res.used++;
            }

            uint64_t sum = res.limbs[pos] + carry;
            carry = (sum < carry) ? 1 : 0;
            res.limbs[pos] = sum;
            pos++;
        }
    }

    // Extract upper half (division by R)
    BigInt result((res.used - k) * 64);
    for (size_t i = 0; i < result.used; i++) {
        result.limbs[i] = res.limbs[i + k];
    }

    result.normalize();

    // Final correction: ensure result < n
    while (!(result < n)) {
        result = result - n;
    }

    return result;
}

BigInt montgomery_mod_pow_raw(BigInt base, BigInt exp, const BigInt& n,
                              int w, std::vector<BigInt>& table,
                              BigInt& R2, uint64_t n0_inv)
{
    // Sliding‑window Montgomery exponentiation.
    //
    // Steps:
    // 1. Convert base to Montgomery domain: base_R = base * R^2 mod n
    // 2. Precompute odd powers: base^(1), base^(3), base^(5), ...
    // 3. Scan exponent bits from MSB to LSB
    // 4. For each window:
    //      - square result for each bit
    //      - multiply by precomputed odd power

    int bits = exp.bit_length();

    BigInt base_R = montgomery_reduce(base * R2, n, n0_inv);
    BigInt result_R = montgomery_reduce(R2, n, n0_inv);

    int table_size = 1 << (w - 1);

    // Precompute odd powers
    table[0] = base_R;
    BigInt base2 = montgomery_reduce(base_R * base_R, n, n0_inv);

    for (int i = 1; i < table_size; i++) {
        table[i] = montgomery_reduce(table[i - 1] * base2, n, n0_inv);
    }

    int i = bits - 1;

    while (i >= 0)
    {
        if (!get_bit(exp, i)) {
            // Bit = 0 → square only
            result_R = montgomery_reduce(result_R * result_R, n, n0_inv);
            i--;
        }
        else {
            // Bit = 1 → find window of consecutive bits
            int j = std::max(i - w + 1, 0);

            while (!get_bit(exp, j)) j++;

            int value = 0;
            for (int k = i; k >= j; k--) {
                value = (value << 1) | get_bit(exp, k);
            }

            int window_size = i - j + 1;

            // Square for each bit in window
            for (int k = 0; k < window_size; k++) {
                result_R = montgomery_reduce(result_R * result_R, n, n0_inv);
            }

            // Multiply by precomputed odd power
            int index = (value - 1) >> 1;
            result_R = montgomery_reduce(result_R * table[index], n, n0_inv);

            i = j - 1;
        }
    }

    return result_R;
}

BigInt montgomery_mod_pow(BigInt base, BigInt exp, const BigInt& n,
                          int w, std::vector<BigInt>& table,
                          BigInt& R2, uint64_t n0_inv)
{
    // Final Montgomery reduction to leave normal domain
    return montgomery_reduce(
        montgomery_mod_pow_raw(base, exp, n, w, table, R2, n0_inv),
        n, n0_inv
    );
}

BigInt extended_gcd_helper(const BigInt& a, const BigInt& b,
                           BigInt& x, BigInt& y,
                           int& signx, int& signy)
{
    // Recursive extended Euclidean algorithm.
    // Tracks signs separately to avoid negative BigInt.

    if (b.isZero()) {
        x = BigInt::one();
        y = BigInt::uint64(0);
        signx = 1;
        signy = 1;
        return a;
    }

    BigInt x1, y1;
    int signx1, signy1;

    BigInt g = extended_gcd_helper(b, a % b, x1, y1, signx1, signy1);

    // Update coefficients
    if (signy1 < 0) signx = -1;
    else signx = 1;
    x = y1;

    BigInt temp = (a / b) * y1;

    // Sign logic is complicated because BigInt is unsigned.
    // We emulate signed arithmetic using separate sign flags.
    if (signx1 > 0) {
        if (x1 < temp && signy1 > 0) {
            y = temp - x1;
            signy = -1;
        }
        else if (signy1 < 0) {
            y = x1 + temp;
            signy = 1;
        }
        else {
            y = x1 - temp;
            signy = 1;
        }
    }
    else {
        if (x1 < temp && signy1 < 0) {
            y = temp - x1;
            signy = 1;
        }
        else if (signy1 < 0) {
            y = x1 - temp;
            signy = -1;
        }
        else {
            y = x1 + temp;
            signy = -1;
        }
    }

    return g;
}

BigInt extended_gcd(const BigInt& a, const BigInt& b, BigInt& x, BigInt& y) {
    // Wrapper for extended GCD.
    // Produces Bézout coefficients x, y such that:
    //     ax + by = gcd(a, b)

    if (b.isZero()) {
        x = BigInt::one();
        y = BigInt::uint64(0);
        return a;
    }

    BigInt x1, y1;
    int signx, signy;

    BigInt g = extended_gcd_helper(b, a % b, x1, y1, signx, signy);

    x = y1;

    BigInt temp = (a / b) * y1;

    // Reconstruct y with correct sign
    if (signy < 0) x = b - x;

    if (signx > 0) {
        if (x1 < temp && signy > 0) {
            y = temp - x1;
            y = a - y;
        }
        else if (signy < 0) {
            y = x1 + temp;
        }
        else {
            y = x1 - temp;
        }
    }
    else {
        if (x1 < temp && signy < 0) {
            y = temp - x1;
        }
        else if (signy < 0) {
            y = x1 - temp;
            y = a - y;
        }
        else {
            y = x1 + temp;
            y = a - y;
        }
    }

    return g;
}

}
