#pragma once

#include <iostream>
#include <vector>
#include <inttypes.h>
#include <string>

class BigInteger
{
public:
	BigInteger(int64_t val = 0);

	BigInteger(const BigInteger & other);

	BigInteger(std::string val, int radix = 10);

	bool operator<(const BigInteger & rhs) const;

	bool operator>(const BigInteger & rhs) const;

	bool operator<=(const BigInteger & rhs) const;

	bool operator>=(const BigInteger & rhs) const;

	bool operator==(const BigInteger & rhs) const;

	bool operator!=(const BigInteger & rhs) const;

	BigInteger operator-() const;

	BigInteger operator+(const BigInteger & rhs) const;

	BigInteger operator-(const BigInteger & rhs) const;

	BigInteger operator*(const BigInteger & rhs) const;

	BigInteger operator/(const BigInteger & rhs) const;

	BigInteger operator%(const BigInteger & rhs) const;

	BigInteger operator<<(int32_t n) const;

	BigInteger operator>>(int32_t n) const;

	BigInteger & operator=(BigInteger rhs);

	BigInteger abs() const;

	BigInteger divide(const BigInteger & val, BigInteger & quotient) const;

	std::string toString() const;

	std::string toString(int radix) const;

	int64_t longValue() const;

	size_t bitLength() const;

	friend std::istream & operator>>(std::istream & input, BigInteger & value);

	friend std::ostream & operator<<(std::ostream & output, const BigInteger & value);

	~BigInteger();

private:
	static const int MIN_RADIX = 2, MAX_RADIX = 36;

	static const int SCHOENHAGE_BASE_CONVERSION_THRESHOLD = 20;

	static const int KNUTH_POW2_THRESH_LEN = 6;

	static const int KNUTH_POW2_THRESH_ZEROS = 3;

	static const std::vector<std::vector<BigInteger>> POWER_CACHE;

	static const std::vector<double> LOG_CACHE;

	static const double LOG_TWO;

	static const int DIGITS_PER_LONG[];

	static const BigInteger LONG_RADIX[];

	static const int DIGITS_PER_INT[];

	static const int INT_RADIX[];

	static const std::vector<std::string> ZEROES;

	static const size_t BITS_PER_DIGIT[];

	class MutableBigInteger
	{
	public:
		std::vector<int32_t> value;

		size_t intLen;

		size_t offset;

		void normalize()
		{
			if (this->intLen == 0)
			{
				this->offset = 0;
				return;
			}
			size_t index = this->offset;
			if (this->value[index] != 0)
			{
				return;
			}
			size_t indexBound = index + this->intLen;
			do {
				index++;
			} while (index < indexBound && this->value[index] == 0);
			size_t numZeros = index - this->offset;
			this->intLen -= numZeros;
			this->offset = this->intLen == 0 ? 0 : this->offset + numZeros;
		}

		BigInteger toBigInteger()
		{
			normalize();
			if (this->intLen == 0)
			{
				return 0;
			}
			return BigInteger(1, std::vector<int32_t>(this->value.begin() + this->offset, this->value.begin() + this->offset + this->intLen));
		}
	};

	int signum;

	std::vector<int32_t> mag;

	BigInteger(std::vector<int32_t> & val);

	BigInteger(int signum, std::vector<int32_t> magnitude);

	BigInteger withSign(int sign) const;

	int32_t getInt(size_t n) const;

	int getLowestSetBit() const;

	int compare(const BigInteger & rhs) const;

	int compareMagnitude(const BigInteger & rhs) const;

	BigInteger shiftRightImpl(int n) const;

	std::string smallToString(int radix) const;

	BigInteger divideKnuth(const BigInteger & val, BigInteger & quotient) const;

	int32_t divideOneWord(int32_t divisor, BigInteger & quotient) const;

	BigInteger divideMagnitude(const BigInteger & div, BigInteger & quotient) const;

	static BigInteger multiplyByInt(const std::vector<int32_t> x, const uint32_t y, int sign);

	static std::vector<int32_t> multiplyToLen(const std::vector<int32_t> x, size_t xlen, const std::vector<int32_t> y, size_t ylen);

	static void copyAndShift(const std::vector<int32_t> & src, size_t srcFrom, size_t srcLen, std::vector<int32_t> & dst, size_t dstFrom, int shift);

	static int64_t divWord(int64_t n, int32_t d);

	static int32_t mulsub(std::vector<int32_t> & q, std::vector<int32_t> & a, int32_t x, size_t len, size_t offset);

	static int32_t mulsubBorrow(std::vector<int32_t> & q, std::vector<int32_t> & a, int32_t x, size_t len, size_t offset);

	static int32_t divadd(std::vector<int32_t> & a, std::vector<int32_t> & result, size_t offset);

	static void toString(const BigInteger & u, std::string & sb, int radix, size_t digits);

	static BigInteger getRadixConversionCache(int radix, int exponent);

	static std::vector<int32_t> add(const std::vector<int32_t> & x, const std::vector<int32_t> & y);

	static std::vector<int32_t> subtract(const std::vector<int32_t> & big, const std::vector<int32_t> & little);

	static std::vector<int32_t> shiftLeft(std::vector<int32_t> mag, int32_t n);

	static std::vector<int32_t> makePositive(const std::vector<int32_t> & val);

	static std::vector<int32_t> stripLeadingZeroInts(const std::vector<int32_t> & val);

	static int bitLengthForInt(int32_t i);

	static int numberOfLeadingZeroes(int32_t i);

	static int numberOfTrailingZeroes(int32_t i);

	static int bitCount(int32_t i);

	static int32_t stringToInt(std::string s, int radix);

	static std::string longToString(int64_t l, int radix);

	static void destructiveMulAdd(std::vector<int32_t> & x, int32_t y, int32_t z);

	static int charToDigit(char ch, int radix);
};
