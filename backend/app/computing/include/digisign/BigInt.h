#pragma once
#include <vector>

namespace digisign {

struct BigInt {

	std::vector<uint64_t> limbs;
	size_t used;

	BigInt();
	BigInt(size_t bits);

	static BigInt uint64(uint64_t number);
	static BigInt one();
	static BigInt two();

	bool isOdd() const;
	bool isZero() const;

	void normalize();
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

	BigInt operator-(uint64_t number) const;

	BigInt operator<<(size_t bit_shift) const;
	BigInt operator>>(size_t bit_shift) const;

};

}