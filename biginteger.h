#pragma once

#include <iostream>
#include <vector>
#include <inttypes.h>

class BigInteger
{
public:
	BigInteger(int64_t val = 0);

	BigInteger(const BigInteger & other);

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

	BigInteger & operator=(BigInteger rhs);

	friend std::istream & operator>>(std::istream & input, const BigInteger & value);

	friend std::ostream & operator<<(std::ostream & output, const BigInteger & value);

	~BigInteger();

private:
	int signum;

	std::vector<int32_t> mag;

	BigInteger(std::vector<int32_t> & val);

	BigInteger(int signum, std::vector<int32_t> magnitude);

	int compare(const BigInteger & rhs) const;

	int compareMagnitude(const BigInteger & rhs) const;

	static std::vector<int32_t> add(const std::vector<int32_t> & x, const std::vector<int32_t> & y);

	static std::vector<int32_t> subtract(const std::vector<int32_t> & big, const std::vector<int32_t> & little);

	static std::vector<int32_t> makePositive(const std::vector<int32_t> & val);

	static std::vector<int32_t> stripLeadingZeroInts(const std::vector<int32_t> & val);
};
