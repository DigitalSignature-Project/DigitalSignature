#include <digisign/BigInt.h>
#include <stdexcept>

namespace digisign {

BigInt::BigInt()
{
    used = 0;
}

BigInt::BigInt(size_t bits)
{
    size_t size = (bits + 63) / 64;
    limbs.resize(size);
    used = size;
}

BigInt BigInt::uint64(uint64_t v)
{
    BigInt r(64);
    r.limbs[0] = v;
    r.used = (v == 0) ? 0 : 1;
    return r;
}

BigInt BigInt::one()
{
    return uint64(1);
}

BigInt BigInt::two()
{
    return uint64(2);
}

bool BigInt::isOdd() const
{
    if (used == 0)
        return false;

    return limbs[0] & 1;
}

bool BigInt::isZero() const
{
    for (size_t i = 0; i < used; i++)
        if (limbs[i] != 0)
            return false;

    return true;
}

void BigInt::normalize()
{
    while (used > 0 && limbs[used - 1] == 0)
        used--;
}

size_t BigInt::bit_length() const
{
    if (used == 0)
        return 0;

    uint64_t last = limbs[used - 1];

    unsigned long index;

    _BitScanReverse64(&index, last);

    return (used - 1) * 64 + index + 1;
}

bool BigInt::operator==(const BigInt& other) const
{
    if (used != other.used)
        return false;

    for (size_t i = 0; i < used; i++)
    {
        if (limbs[i] != other.limbs[i])
            return false;
    }

    return true;
}

bool BigInt::operator!=(const BigInt& other) const
{
    return !(*this == other);
}

bool BigInt::operator<(const BigInt& other) const
{
    if (used != other.used)
        return used < other.used;

    for (int i = used - 1; i >= 0; i--)
    {
        if (limbs[i] != other.limbs[i])
            return limbs[i] < other.limbs[i];
    }

    return false;
}

bool BigInt::operator>(const BigInt& other) const
{
    return other < *this;
}

BigInt BigInt::operator+(const BigInt& other) const
{
    const size_t max_size = std::max(used, other.used);

    BigInt result(max_size*64);

    uint64_t carry = 0;

    for (size_t i = 0; i < max_size; i++)
    {
        uint64_t a = (i < used) ? limbs[i] : 0;
        uint64_t b = (i < other.used) ? other.limbs[i] : 0;

        uint64_t sum = a + b + carry;

        result.limbs[i] = sum;

        if (sum < a || (carry && sum == a))
            carry = 1;
        else
            carry = 0;
    }

    if (carry) {
        result.limbs.push_back(carry);
        result.used++;
    }

    result.normalize();

    return result;
}

BigInt BigInt::operator-(const BigInt& other) const
{
    if (*this < other)
        throw std::exception("First operand must be bigger than the second");

    size_t max_size = used;

    BigInt result(max_size*64);

    uint64_t borrow = 0;

    for (size_t i = 0; i < max_size; i++)
    {
        uint64_t a = (i < used) ? limbs[i] : 0;
        uint64_t b = (i < other.used) ? other.limbs[i] : 0;

        uint64_t sub;

        if (a >= b + borrow && !(borrow && b == UINT64_MAX))
        {
            sub = a - b - borrow;
            borrow = 0;
        }
        else
        {
            sub = a + (UINT64_MAX - b - borrow) + 1;
            borrow = 1;
        }

        result.limbs[i] = sub;
    }

    result.normalize();

    return result;
}

BigInt BigInt::operator*(const BigInt& other) const
{
    size_t n = used;
    size_t m = other.used;

    BigInt result((n+m)*64);

    for (size_t i = 0; i < n; i++)
    {
        uint64_t carry = 0;

        for (size_t j = 0; j < m; j++)
        {
            uint64_t high;
            uint64_t low = _umul128(limbs[i], other.limbs[j], &high);

            uint64_t temp = result.limbs[i + j];

            uint64_t sum = temp + low;
            uint64_t carry1 = (sum < temp);

            sum += carry;
            uint64_t carry2 = (sum < carry);

            result.limbs[i + j] = sum;

            carry = high + carry1 + carry2;
        }

        result.limbs[i + m] += carry;
    }

    result.normalize();

    return result;
}

BigInt BigInt::operator<<(size_t shift) const
{
    if (shift == 0) return *this;

    size_t limb_shift = shift / 64;
    size_t bit_shift = shift % 64;

    BigInt result((used + limb_shift + 1)*64);

    uint64_t carry = 0;

    for (size_t i = 0; i < used; i++)
    {
        uint64_t value = limbs[i];

        result.limbs[i + limb_shift] |= value << bit_shift;

        if (bit_shift != 0)
        {
            result.limbs[i + limb_shift] |= carry;

            carry = value >> (64 - bit_shift);

            result.limbs[i + limb_shift + 1] |= carry;
        }
    }

    result.normalize();
    return result;
}

BigInt BigInt::operator>>(size_t shift) const
{
    if (shift == 0) return *this;

    size_t limb_shift = shift / 64;
    size_t bit_shift = shift % 64;

    if (limb_shift >= used)
        return BigInt(0);

    BigInt result((used - limb_shift) * 64);

    uint64_t carry = 0;

    for (int i = used - 1; i >= (int)limb_shift; i--)
    {
        uint64_t value = limbs[i];

        result.limbs[i - limb_shift] = value >> bit_shift;

        if (bit_shift != 0)
        {
            result.limbs[i - limb_shift] |= carry;

            carry = value << (64 - bit_shift);
        }
    }

    result.normalize();
    return result;
}   

BigInt BigInt::operator%(const BigInt& mod) const
{
    if (mod.isZero())
        throw std::runtime_error("Modulo by zero");

    if (*this < mod)
        return *this;

    BigInt remainder = *this;

    size_t shift = remainder.bit_length() - mod.bit_length();

    for (size_t i = shift + 1; i-- > 0;)
    {
        BigInt divisor = mod << i;

        if (!(remainder < divisor))
            remainder = remainder - divisor;
    }

    remainder.normalize();

    return remainder;
}

BigInt BigInt::operator/(const BigInt& div) const
{
    if (div.isZero())
        throw std::runtime_error("Division by zero");

    if (*this < div)
        return BigInt::uint64(0);

    BigInt quotient(used * 64);
    BigInt remainder = *this;

    size_t shift = remainder.bit_length() - div.bit_length();

    for (size_t i = shift + 1; i-- > 0;)
    {
        BigInt divisor = div << i;

        if (!(remainder < divisor)) {
            remainder = remainder - divisor;
                    
            size_t limb = i / 64;
            size_t bit = i % 64;

            if (limb >= quotient.limbs.size())
                quotient.limbs.resize(limb + 1, 0);

            quotient.limbs[limb] |= (1ULL << bit);
        }
    }

    quotient.normalize();

    return quotient;
}

BigInt BigInt::operator-(uint64_t v) const
{
    BigInt result = *this;

    uint64_t borrow = v;

    for (size_t i = 0; i < result.used && borrow; i++)
    {
        uint64_t old = result.limbs[i];
        result.limbs[i] -= borrow;

        borrow = (result.limbs[i] > old);
    }

    if (borrow)
        throw std::runtime_error("BigInt underflow");

    result.normalize();

    return result;
}	

}