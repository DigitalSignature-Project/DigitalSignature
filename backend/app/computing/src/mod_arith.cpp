#include <digisign/mod_arith.h>
#include <stdexcept>

namespace digisign {

uint32_t small_mod_barrett(const BigInt& n, uint32_t p, uint64_t R_mod_p, uint64_t mu) {
    uint64_t result = 0;

    for (int i = n.used - 1; i >= 0; i--) {
        uint64_t limb = n.limbs[i];

        uint64_t hi1, lo1;
        lo1 = _umul128(result, R_mod_p, &hi1);

        uint64_t lo = lo1 + limb;
        uint64_t hi = hi1 + (lo < limb ? 1 : 0);

        uint64_t hi2, lo2;
        lo2 = _umul128(lo, mu, &hi2);
        uint64_t q = hi * mu + hi2;

        lo2 = _umul128(q, p, &hi2);

        uint64_t r = lo - lo2;
        if (r > lo) hi2++;

        r = r + hi - hi2;

        if (r >= p) r -= p;
        if (r >= p) r -= p;

        result = r;
    }

    return (uint32_t)result;
}

BigInt mod_pow(const BigInt& base, const BigInt& exp, const BigInt& mod) {
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
    uint64_t n0 = n.limbs[0];
    uint64_t x = 1;
    for (int i = 0; i < k; i++) {
        x = x * (2 - n0 * x);
    }
    return (~x) + 1;
}

BigInt montgomery_reduce(const BigInt& t, const BigInt& n, uint64_t n_inv) {

    if (t.used > 2 * n.used) {
        throw std::runtime_error("t mustn't have more limbs than 2x n's limbs");
    }

    size_t k = n.used;

    BigInt res = t;
    res.limbs.resize(2 * k);
    res.used = 2 * k;

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

    BigInt result((res.used - k) * 64);
    for (size_t i = 0; i < result.used; i++) {
        result.limbs[i] = res.limbs[i + k];
    }

    result.normalize();

    while (!(result < n)) {
        result = result - n;
    }

    return result;
}

BigInt montgomery_mod_pow_raw(BigInt base, BigInt exp, const BigInt& n, int w, std::vector<BigInt>& table, BigInt& R2, uint64_t n0_inv)
{
    size_t n_limbs = n.used;

    int bits = exp.bit_length();

    BigInt base_R = montgomery_reduce(base * R2, n, n0_inv);
    BigInt result_R = montgomery_reduce(R2, n, n0_inv);

    int table_size = 1 << (w - 1);

    table[0] = base_R;

    BigInt base2 = montgomery_reduce(base_R * base_R, n, n0_inv);

    for (int i = 1; i < table_size; i++) {
        table[i] = montgomery_reduce(table[i - 1] * base2, n, n0_inv);
    }

    int i = bits - 1;

    while (i >= 0)
    {
        if (!get_bit(exp, i)) {
            result_R = montgomery_reduce(result_R * result_R, n, n0_inv);
            i--;
        }
        else {
            int j = std::max(i - w + 1, 0);

            while (!get_bit(exp, j)) j++;

            int value = 0;
            for (int k = i; k >= j; k--) {
                value = (value << 1) | get_bit(exp, k);
            }

            int window_size = i - j + 1;

            for (int k = 0; k < window_size; k++) {
                result_R = montgomery_reduce(result_R * result_R, n, n0_inv);
            }

            int index = (value - 1) >> 1;
            result_R = montgomery_reduce(result_R * table[index], n, n0_inv);

            i = j - 1;
        }
    }

    return result_R;
}

BigInt montgomery_mod_pow(BigInt base, BigInt exp, const BigInt& n, int w, std::vector<BigInt>& table, BigInt& R2, uint64_t n0_inv) {
    return montgomery_reduce(montgomery_mod_pow_raw(base, exp, n, w, table, R2, n0_inv), n, n0_inv);
}

BigInt extended_gcd_helper(const BigInt& a, const BigInt& b, BigInt& x, BigInt& y, int& signx, int& signy) {
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

    if (signy1 < 0) signx = -1;
    else signx = 1;
    x = y1;

    BigInt temp = (a / b) * y1;
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