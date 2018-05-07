#include <utility>

#include "stdafx.h"
#include "biginteger.h"

BigInteger::BigInteger(int64_t val)
{
	if (val < 0)
	{
		val = -val;
		this->signum = -1;
	}
	else
	{
		this->signum = 1;
	}
	int32_t highWord = (int32_t)((uint64_t)val >> 32);
	if (highWord != 0)
	{
		this->mag.push_back(highWord);
	}
	this->mag.push_back((int32_t)val);
}

BigInteger::BigInteger(const BigInteger & other) : signum(other.signum), mag(other.mag)
{
}

BigInteger::BigInteger(std::vector<int32_t> & val)
{
	if (val.empty())
	{
		throw "Zero length BigInteger";
	}
	if (val[0] < 0)
	{
		this->mag = makePositive(val);
		this->signum = -1;
	}
	else
	{
		this->mag = stripLeadingZeroInts(val);
		this->signum = mag.size() == 0 ? 0 : 1;
	}
}

BigInteger::BigInteger(int signum, std::vector<int32_t> magnitude)
{
	this->signum = magnitude.size() == 0 ? 0 : signum;
	this->mag = magnitude;
}

bool BigInteger::operator<(const BigInteger & rhs) const
{
	return compare(rhs) < 0;
}

bool BigInteger::operator>(const BigInteger & rhs) const
{
	return compare(rhs) > 0;
}

bool BigInteger::operator<=(const BigInteger & rhs) const
{
	return compare(rhs) <= 0;
}

bool BigInteger::operator>=(const BigInteger & rhs) const
{
	return compare(rhs) >= 0;
}

bool BigInteger::operator==(const BigInteger & rhs) const
{
	return this->signum == rhs.signum && this->mag == rhs.mag;
}

bool BigInteger::operator!=(const BigInteger & rhs) const
{
	return !(*this == rhs);
}

BigInteger BigInteger::operator-() const
{
	return BigInteger(-this->signum, this->mag);
}

BigInteger BigInteger::operator+(const BigInteger & rhs) const
{
	if (rhs.signum == 0)
	{
		return *this;
	}
	if (this->signum == 0)
	{
		return rhs;
	}
	if (rhs.signum == this->signum)
	{
		return BigInteger(this->signum, add(this->mag, rhs.mag));
	}
	int cmp = compareMagnitude(rhs);
	if (cmp == 0)
	{
		return BigInteger(0);
	}
	std::vector<int32_t> resultMag = cmp > 0 ? subtract(this->mag, rhs.mag) : subtract(rhs.mag, this->mag);
	resultMag = stripLeadingZeroInts(resultMag);
	return BigInteger(cmp == this->signum ? 1 : -1, resultMag);
}

BigInteger BigInteger::operator-(const BigInteger & rhs) const
{
	if (rhs.signum == 0)
	{
		return *this;
	}
	if (this->signum == 0)
	{
		return -*this;
	}
	if (rhs.signum != this->signum)
	{
		return BigInteger(this->signum, add(this->mag, rhs.mag));
	}
	int cmp = compareMagnitude(rhs);
	if (cmp == 0)
	{
		return BigInteger(0);
	}
	std::vector<int32_t> resultMag = cmp > 0 ? subtract(this->mag, rhs.mag) : subtract(rhs.mag, this->mag);
	resultMag = stripLeadingZeroInts(resultMag);
	return BigInteger(cmp == this->signum ? 1 : -1, resultMag);
}

BigInteger BigInteger::operator*(const BigInteger & rhs) const
{
	return BigInteger();
}

BigInteger & BigInteger::operator=(BigInteger rhs)
{
	std::swap(this->signum, rhs.signum);
	std::swap(this->mag, rhs.mag);
	return *this;
}

int BigInteger::compare(const BigInteger & rhs) const
{
	if (this->signum == rhs.signum)
	{
		switch (this->signum)
		{
		case 1:
			return compareMagnitude(rhs);
		case -1:
			return rhs.compareMagnitude(*this);
		default:
			return 0;
		}
	}
	return this->signum > rhs.signum ? 1 : -1;
}

int BigInteger::compareMagnitude(const BigInteger & rhs) const
{
	std::vector<int32_t> m1 = this->mag;
	size_t len1 = m1.size();
	std::vector<int32_t> m2 = rhs.mag;
	size_t len2 = m2.size();
	if (len1 < len2)
	{
		return -1;
	}
	if (len1 > len2)
	{
		return 1;
	}
	for (size_t i = 0; i < len1; i++)
	{
		int32_t a = m1[i];
		int32_t b = m2[i];
		if (a != b)
		{
			return (uint32_t)a < (uint32_t)b ? -1 : 1;
		}
	}
	return 0;
}

BigInteger::~BigInteger()
{
}

std::istream & operator>>(std::istream & input, const BigInteger & value)
{
	return input;
}

std::ostream & operator<<(std::ostream & output, const BigInteger & value)
{
	return output;
}

std::vector<int32_t> BigInteger::add(const std::vector<int32_t> & x, const std::vector<int32_t> & y)
{
	if (x.size() < y.size())
	{
		return add(y, x);
	}
	size_t xIndex = x.size();
	size_t yIndex = y.size();
	std::vector<int32_t> result(xIndex);
	int64_t sum = 0;
	if (yIndex == 1)
	{
		sum = (uint32_t)x[--xIndex] + (uint32_t)y[0];
		result[xIndex] = (int32_t)sum;
	}
	else
	{
		while (yIndex > 0)
		{
			sum = (uint32_t)x[--xIndex] + (uint32_t)y[--yIndex] + ((uint64_t)sum >> 32);
			result[xIndex] = (int32_t)sum;
		}
	}
	bool carry = ((uint64_t)sum >> 32) != 0;
	while (xIndex > 0 && carry)
	{
		carry = (result[--xIndex] = x[xIndex] + 1) == 0;
	}
	while (xIndex > 0)
	{
		result[--xIndex] = x[xIndex];
	}
	if (carry)
	{
		result.insert(result.begin(), 1);
	}
	return result;
}

std::vector<int32_t> BigInteger::subtract(const std::vector<int32_t> & big, const std::vector<int32_t> & little)
{
	size_t bigIndex = big.size();
	std::vector<int32_t> result(bigIndex);
	size_t littleIndex = little.size();
	int64_t difference = 0;
	while (littleIndex > 0)
	{
		difference = (uint32_t)big[--bigIndex] - (uint32_t)little[--littleIndex] + (difference >> 32);
	}
	bool borrow = (difference >> 32) != 0;
	while (bigIndex > 0 && borrow)
	{
		borrow = (result[--bigIndex] = big[bigIndex] - 1) == -1;
	}
	while (bigIndex > 0)
	{
		result[--bigIndex] = big[bigIndex];
	}
	return result;
}

std::vector<int32_t> BigInteger::makePositive(const std::vector<int32_t> & a)
{
	size_t keep, j;
	for (keep = 0; keep < a.size() && a[keep] == -1; keep++);
	for (j = keep; j < a.size() && a[j] == 0; j++);
	int extraInt = j == a.size() ? 1 : 0;
	std::vector<int32_t> result(a.size() - keep + extraInt);
	for (auto i = keep; i < a.size(); i++)
	{
		result[i - keep + extraInt] = ~a[i];
	}
	for (auto i = result.size() - 1; ++result[i] == 0; i--);
	return result;
}

std::vector<int32_t> BigInteger::stripLeadingZeroInts(const std::vector<int32_t> & val)
{
	size_t keep;
	for (keep = 0; keep < val.size() && val[keep] == 0; keep++);
	return std::vector<int32_t>(val.begin() + keep, val.end());
}
