#pragma once

#include <digisign/BigInt.h>

namespace digisign {

uint32_t small_mod_barrett(const BigInt& n, uint32_t small_prime, uint64_t R_mod_p, uint64_t mu);

BigInt mod_pow(const BigInt& base, const BigInt& exp, const BigInt& mod);

BigInt montgomery_reduce(const BigInt& a, const BigInt& n, uint64_t inverse_mod_n);

uint64_t n_inv(const BigInt& n, int power_of_two);

BigInt montgomery_mod_pow_raw(BigInt base, BigInt exp, const BigInt& mod, int window_bit_size, std::vector<BigInt>& table_reference, BigInt& R2, uint64_t inverse_mod_n);

BigInt montgomery_mod_pow(BigInt base, BigInt exp, const BigInt& mod, int window_bit_size, std::vector<BigInt>& table_reference, BigInt& R2, uint64_t inverse_mod_n);

BigInt extended_gcd_helper(const BigInt& a, const BigInt& b, BigInt& x, BigInt& y, int& signx, int& signy);

BigInt extended_gcd(const BigInt& a, const BigInt& b, BigInt& x, BigInt& y);

inline bool get_bit(const BigInt& x, int i) {
    return (x.limbs[i / 64] >> (i % 64)) & 1;
}

}