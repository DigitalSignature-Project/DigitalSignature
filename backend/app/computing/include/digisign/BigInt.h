#pragma once
#include <vector>
#include <string>

namespace digisign {

/**
 * @brief Arbitrary‑precision unsigned integer (little‑endian 64‑bit limbs).
 *
 * BigInt is the fundamental building block for all cryptographic operations.
 * It stores the number as a vector of 64‑bit limbs in little‑endian order:
 *   limbs[0] = least significant 64 bits
 *   limbs[used-1] = most significant non‑zero limb
 *
 * The class provides:
 *  - basic arithmetic (+, -, *, /, %)
 *  - bit shifts
 *  - conversion to/from hex, base64, byte vectors
 *  - normalization (removal of leading zero limbs)
 *
 * NOTE:
 *  - All operations assume non‑negative integers.
 *  - Division and modulo use simple long division (not optimized).
 *  - Multiplication uses 128‑bit intermediate products.
 */
struct BigInt {

    /// 64‑bit limbs, little‑endian (limbs[0] = least significant)
    std::vector<uint64_t> limbs;

    /// Number of limbs actually used (ignores trailing zeros)
    size_t used;

    /** @brief Constructs an empty BigInt with value 0. */
    BigInt();

    /**
     * @brief Constructs a BigInt with enough space for `bits` bits.
     * @param bits Number of bits to reserve.
     */
    BigInt(size_t bits);

    /** @brief Creates a BigInt from a 64‑bit integer. */
    static BigInt uint64(uint64_t number);

    /** @brief Creates a BigInt from a big‑endian byte vector. */
    static BigInt vectoruint8(const std::vector<uint8_t>& v);

    /** @brief Parses a BigInt from a hex string. */
    static BigInt from_hex(const std::string& s);

    /** @brief Parses a BigInt from a base64 string. */
    static BigInt from_base64(const std::string& s);

    /** @brief Returns BigInt(1). */
    static BigInt one();

    /** @brief Returns BigInt(2). */
    static BigInt two();

    /** @brief Converts BigInt to a big‑endian byte vector. */
    std::vector<uint8_t> to_vectoruint8() const;

    /** @brief Returns true if the number is odd. */
    bool isOdd() const;

    /** @brief Returns true if the number is zero. */
    bool isZero() const;

    /** @brief Removes leading zero limbs. */
    void normalize();

    /** @brief Returns the bit length of the number. */
    size_t bit_length() const;

    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;
    bool operator<(const BigInt& other) const;
    bool operator>(const BigInt& other) const;

    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;

    /** @brief Subtracts a 64‑bit integer. */
    BigInt operator-(uint64_t number) const;

    /** @brief Left bit‑shift. */
    BigInt operator<<(size_t bit_shift) const;

    /** @brief Right bit‑shift. */
    BigInt operator>>(size_t bit_shift) const;

    /** @brief Converts BigInt to hex string. */
    std::string to_hex(bool remove_leading_zeros = false) const;

    /** @brief Converts BigInt to base64 string. */
    std::string to_base64() const;
};

}
