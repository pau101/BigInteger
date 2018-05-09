#include <utility>

#include <algorithm>

#include "stdafx.h"
#include "biginteger.h"

const std::vector<std::vector<BigInteger>> BigInteger::POWER_CACHE = [] {
	std::vector<std::vector<BigInteger>> powerCache(MAX_RADIX + 1);
	for (auto i = MIN_RADIX; i <= MAX_RADIX; i++)
	{
		powerCache[i].push_back(BigInteger(i));
	}
	return powerCache;
}();

const std::vector<double> BigInteger::LOG_CACHE = [] {
	std::vector<double> logCache(MAX_RADIX + 1);
	for (auto i = MIN_RADIX; i <= MAX_RADIX; i++)
	{
		logCache[i] = std::log(i);
	}
	return logCache;
}();

const double BigInteger::LOG_TWO = std::log(2.0);

const int BigInteger::DIGITS_PER_LONG[] = { 0, 0,
	62, 39, 31, 27, 24, 22, 20, 19, 18, 18, 17, 17, 16, 16, 15, 15, 15, 14,
	14, 14, 14, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12, 12, 12
};

const BigInteger BigInteger::LONG_RADIX[] = { NULL, NULL,
	BigInteger(0x4000000000000000), BigInteger(0x383d9170b85ff80b),
	BigInteger(0x4000000000000000), BigInteger(0x6765c793fa10079d),
	BigInteger(0x41c21cb8e1000000), BigInteger(0x3642798750226111),
	BigInteger(0x1000000000000000), BigInteger(0x12bf307ae81ffd59),
	BigInteger( 0xde0b6b3a7640000), BigInteger(0x4d28cb56c33fa539),
	BigInteger(0x1eca170c00000000), BigInteger(0x780c7372621bd74d),
	BigInteger(0x1e39a5057d810000), BigInteger(0x5b27ac993df97701),
	BigInteger(0x1000000000000000), BigInteger(0x27b95e997e21d9f1),
	BigInteger(0x5da0e1e53c5c8000), BigInteger( 0xb16a458ef403f19),
	BigInteger(0x16bcc41e90000000), BigInteger(0x2d04b7fdd9c0ef49),
	BigInteger(0x5658597bcaa24000), BigInteger( 0x6feb266931a75b7),
	BigInteger( 0xc29e98000000000), BigInteger(0x14adf4b7320334b9),
	BigInteger(0x226ed36478bfa000), BigInteger(0x383d9170b85ff80b),
	BigInteger(0x5a3c23e39c000000), BigInteger( 0x4e900abb53e6b71),
	BigInteger( 0x7600ec618141000), BigInteger( 0xaee5720ee830681),
	BigInteger(0x1000000000000000), BigInteger(0x172588ad4f5f0981),
	BigInteger(0x211e44f7d02c1000), BigInteger(0x2ee56725f06e5c71),
	BigInteger(0x41c21cb8e1000000)
};

const std::vector<std::string> BigInteger::ZEROES = [] {
	std::vector<std::string> zeroes(64);
	zeroes[63] = "000000000000000000000000000000000000000000000000000000000000000";
	for (auto i = 0; i < 63; i++)
	{
		zeroes[i] = zeroes[63].substr(0, i);
	}
	return zeroes;
}();

BigInteger::BigInteger(int64_t val)
{
	if (val == 0)
	{
		this->signum = 0;
	}
	else
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
}

BigInteger::BigInteger(const BigInteger & other) :
	signum(other.signum),
	mag(other.mag)
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

BigInteger BigInteger::withSign(int sign) const
{
	return this->signum == 0 ? *this : BigInteger(sign, this->mag);
}

int32_t BigInteger::getInt(size_t n) const
{
	if (n < 0)
	{
		return 0;
	}
	if (n >= this->mag.size())
	{
		return this->signum < 0 ? -1 : 0;
	}
	int32_t magInt = this->mag[this->mag.size() - n - 1];
	if (this->signum >= 0)
	{
		return magInt;
	}
	size_t i;
	size_t mlen = this->mag.size();
	for (i = mlen - 1; i >= 0 && this->mag[i] == 0; i--);
	if (n <= mlen - i - 1)
	{
		return -magInt;
	}
	return ~magInt;
}

int BigInteger::getLowestSetBit() const
{
	if (this->mag.size() == 0)
	{
		return -1;
	}
	size_t j;
	for (j = this->mag.size() - 1; j > 0 && this->mag[j] == 0; j--);
	int32_t b = this->mag[j];
	if (b == 0)
	{
		return -1;
	}
	return ((this->mag.size() - 1 - j) << 5) + numberOfTrailingZeroes(b);
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
		return 0;
	}
	std::vector<int32_t> resultMag = cmp > 0 ? subtract(this->mag, rhs.mag) : subtract(rhs.mag, this->mag);
	resultMag = stripLeadingZeroInts(resultMag);
	return BigInteger(cmp == this->signum ? 1 : -1, resultMag);
}

BigInteger BigInteger::operator*(const BigInteger & rhs) const
{
	if (rhs.signum == 0 || this->signum == 0)
	{
		return 0;
	}
	size_t xlen = this->mag.size();
	size_t ylen = rhs.mag.size();
	int resultSign = this->signum == rhs.signum ? 1 : -1;
	if (rhs.mag.size() == 1)
	{
		return multiplyByInt(this->mag, rhs.mag[0], resultSign);
	}
	if (this->mag.size() == 1)
	{
		return multiplyByInt(rhs.mag, this->mag[0], resultSign);
	}
	std::vector<int32_t> result = multiplyToLen(this->mag, xlen, rhs.mag, ylen);
	result = stripLeadingZeroInts(result);
	return BigInteger(resultSign, result);
}

BigInteger BigInteger::operator/(const BigInteger & rhs) const
{
	BigInteger q;
	divide(rhs, q);
	return q;
}

BigInteger BigInteger::operator%(const BigInteger & rhs) const
{
	BigInteger q;
	return divide(rhs, q);
}

BigInteger BigInteger::operator<<(int32_t n) const
{
	if (this->signum == 0)
	{
		return BigInteger(0);
	}
	if (n > 0)
	{
		return BigInteger(this->signum, shiftLeft(this->mag, n));
	}
	if (n == 0)
	{
		return *this;
	}
	return shiftRightImpl(-n);
}

BigInteger BigInteger::operator>>(int32_t n) const
{
	if (this->signum == 0)
	{
		return BigInteger(0);
	}
	if (n > 0)
	{
		return shiftRightImpl(n);
	}
	if (n == 0)
	{
		return *this;
	}
	return BigInteger(this->signum, shiftLeft(this->mag, -n));
}

BigInteger & BigInteger::operator=(BigInteger rhs)
{
	std::swap(this->signum, rhs.signum);
	std::swap(this->mag, rhs.mag);
	return *this;
}

BigInteger BigInteger::abs() const
{
	return this->signum >= 0 ? *this : -*this;
}

BigInteger BigInteger::divide(const BigInteger & val, BigInteger & quotient) const
{
	BigInteger r = divideKnuth(val, quotient);
	quotient = quotient.withSign(this->signum == val.signum ? 1 : -1);
	return r.withSign(this->signum);
}

std::string BigInteger::toString() const
{
	return toString(10);
}

std::string BigInteger::toString(int radix) const
{
	if (this->signum == 0)
	{
		return "0";
	}
	if (radix < MIN_RADIX || radix > MAX_RADIX)
	{
		radix = 10;
	}
	if (this->mag.size() < SCHOENHAGE_BASE_CONVERSION_THRESHOLD)
	{
		return smallToString(radix);
	}
	std::string sb;
	if (this->signum < 0)
	{
		toString(-*this, sb, radix, 0);
		sb.insert(0, "-");
	}
	else
	{
		toString(*this, sb, radix, 0);
	}
	return sb;
}

int64_t BigInteger::longValue() const
{
	int64_t result = 0;
	for (size_t i = 2; i-- > 0; )
	{
		result = (result << 32) + (uint32_t)getInt(i);
	}
	return result;
}

size_t BigInteger::bitLength() const
{
	int n;
	std::vector<int32_t> m = this->mag;
	size_t len = m.size();
	if (len == 0)
	{
		n = 0;
	}
	else
	{
		int magBitLength = ((len - 1) << 5) + bitLengthForInt(m[0]);
		if (this->signum < 0)
		{
			bool pow2 = bitCount(m[0]) == 1;
			for (size_t i = 1; i < len && pow2; i++)
			{
				pow2 = m[i] == 0;
			}
			n = pow2 ? magBitLength - 1 : magBitLength;
		}
		else
		{
			n = magBitLength;
		}
	}
	return n;
}

BigInteger::~BigInteger()
{
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

BigInteger BigInteger::shiftRightImpl(int n) const
{
	size_t nInts = (uint32_t)n >> 5;
	int nBits = n & 0x1F;
	size_t magLen = mag.size();
	std::vector<int32_t> newMag;
	if (nInts >= magLen)
	{
		return this->signum >= 0 ? BigInteger(0) : BigInteger(-1);
	}
	if (nBits == 0)
	{
		newMag.insert(newMag.begin(), mag.begin(), mag.begin() + (magLen - nInts) + 1);
	}
	else
	{
		int i = 0;
		int32_t highBits = (uint32_t)mag[0] >> nBits;
		if (highBits != 0)
		{
			newMag.assign(magLen - nInts, 0);
			newMag[i++] = highBits;
		}
		else
		{
			newMag.assign(magLen - nInts - 1, 0);
		}
		int nBits2 = 32 - nBits;
		size_t j = 0;
		while (j < magLen - nInts - 1)
		{
			newMag[i++] = mag[j++] << nBits2 | (uint32_t)mag[j] >> nBits;
		}
	}
	if (this->signum < 0)
	{
		bool onesLost = false;
		for (size_t i = magLen, j = magLen - nInts; i-- > j && !onesLost; )
		{
			onesLost = this->mag[i] != 0;
		}
		if (!onesLost && nBits != 0)
		{
			onesLost = (this->mag[magLen - nInts - 1] << (32 - nBits)) != 0;
		}
		if (onesLost)
		{
			int32_t lastSum = 0;
			for (size_t i = newMag.size(); i-- > 0 && lastSum == 0; )
			{
				lastSum = (newMag[i] += 1);
			}
			if (lastSum == 0)
			{
				newMag = std::vector<int32_t>(newMag.size() + 1);
				newMag[0] = 1;
			}
		}
	}
	return BigInteger(this->signum, newMag);
}

std::string BigInteger::smallToString(int radix) const
{
	if (this->signum == 0)
	{
		return "0";
	}
	int maxNumDigitGroups = (4 * this->mag.size() + 6) / 7;
	std::vector<std::string> digitGroup(maxNumDigitGroups);
	BigInteger tmp = this->abs();
	int numGroups = 0;
	while (tmp.signum != 0)
	{
		BigInteger d = LONG_RADIX[radix];
		BigInteger q;
		BigInteger r = tmp.divide(d, q);
		digitGroup[numGroups++] = lltoa(r.longValue(), radix);
		tmp = q;
	}
	std::string buf;
	buf.reserve(numGroups * DIGITS_PER_LONG[radix] + 1);
	if (this->signum < 0)
	{
		buf.append("-");
	}
	buf.append(digitGroup[numGroups - 1]);
	for (int i = numGroups - 2; i >= 0; i--)
	{
		int numLeadingZeroes = DIGITS_PER_LONG[radix] - digitGroup[i].size();
		if (numLeadingZeroes != 0)
		{
			buf.append(ZEROES[numLeadingZeroes]);
		}
		buf.append(digitGroup[i]);
	}
	return buf;
}

BigInteger BigInteger::divideKnuth(const BigInteger & b, BigInteger & quotient) const
{
	if (b.mag.size() == 0)
	{
		throw "BigInteger divide by zero";
	}
	if (this->mag.size() == 0)
	{
		quotient = 0;
		return 0;
	}
	int cmp = compare(b);
	if (cmp < 0)
	{
		quotient = 0;
		return *this;
	}
	if (cmp == 0)
	{
		quotient = 1;
		return 0;
	}
	if (b.mag.size() == 1)
	{
		return divideOneWord(b.mag[0], quotient);
	}
	if (false && this->mag.size() >= KNUTH_POW2_THRESH_LEN)
	{
		int lb1 = getLowestSetBit(), lb2 = b.getLowestSetBit();
		int trailingZeroBits = lb1 < lb2 ? lb1 : lb2;
		if (trailingZeroBits >= KNUTH_POW2_THRESH_ZEROS * 32)
		{
			BigInteger r = (*this >> trailingZeroBits).divide(b >> trailingZeroBits, quotient);
			r = r << trailingZeroBits;
			return r;
		}
	}
	return divideMagnitude(b, quotient);
}

int32_t BigInteger::divideOneWord(int32_t divisor, BigInteger & quotient) const
{
	uint32_t divisorLong = (uint32_t)divisor;
	if (this->mag.size() == 1)
	{
		uint32_t dividendValue = (uint32_t)this->mag[0];
		int32_t q = (int32_t)(dividendValue / divisorLong);
		int32_t r = (int32_t)(dividendValue - q * divisorLong);
		quotient = q;
		return r;
	}
	MutableBigInteger q;
	q.value.assign(this->mag.size(), 0);
	q.offset = 0;
	q.intLen = this->mag.size();
	int shift = numberOfLeadingZeroes(divisor);
	int32_t rem = this->mag[0];
	uint32_t remLong = (uint32_t)rem;
	if (remLong < divisorLong)
	{
		q.value[0] = 0;
	}
	else
	{
		q.value[0] = (int32_t)(remLong / divisorLong);
		rem = (int32_t)(remLong - q.value[0] * divisorLong);
		remLong = (uint32_t)rem;
	}
	size_t xlen = this->mag.size();
	while (--xlen > 0)
	{
		int64_t dividendEstimate = ((uint64_t)remLong << 32) | (uint32_t)this->mag[this->mag.size() - xlen];
		int32_t qhat;
		if (dividendEstimate >= 0)
		{
			qhat = (int32_t)(dividendEstimate / divisorLong);
			rem = (int32_t)(dividendEstimate - qhat * divisorLong);
		}
		else
		{
			int64_t tmp = divWord(dividendEstimate, divisor);
			qhat = (int32_t)tmp;
			rem = (int32_t)((uint64_t)tmp >> 32);
		}
		q.value[this->mag.size() - xlen] = qhat;
		remLong = (int32_t)rem;
	}
	quotient = q.toBigInteger();
	if (shift > 0)
	{
		return rem % divisor;
	}
	return rem;
}

BigInteger BigInteger::divideMagnitude(const BigInteger & div, BigInteger & quotient) const
{
	if (div.mag.size() <= 1)
	{
		throw "Illegal divisor";
	}
	int shift = numberOfLeadingZeroes(div.mag[0]);
	size_t dlen = div.mag.size();
	std::vector<int32_t> divisor;
	MutableBigInteger rem;
	if (shift > 0)
	{
		divisor.assign(dlen, 0);
		copyAndShift(div.mag, 0, dlen, divisor, 0, shift);
		if (numberOfLeadingZeroes(this->mag[0]) >= shift)
		{
			rem.value.assign(this->mag.size() + 1, 0);
			rem.intLen = this->mag.size();
			rem.offset = 1;
			copyAndShift(this->mag, 0, this->mag.size(), rem.value, 1, shift);
		}
		else
		{
			rem.value.assign(this->mag.size() + 2, 0);
			rem.intLen = this->mag.size() + 1;
			rem.offset = 1;
			size_t rFrom = 0;
			int32_t c = 0;
			int n2 = 32 - shift;
			for (size_t i = 1; i < this->mag.size() + 1; i++, rFrom++)
			{
				int32_t b = c;
				c = this->mag[rFrom];
				rem.value[i] = (b << shift) | ((uint32_t)c >> n2);
			}
			rem.value[this->mag.size() + 1] = c << shift;
		}
	}
	else
	{
		divisor = div.mag;
		rem.value.insert(rem.value.begin(), this->mag.begin(), this->mag.end());
		rem.value.insert(rem.value.begin(), 0);
		rem.intLen = this->mag.size();
		rem.offset = 1;
	}
	size_t nlen = rem.intLen;
	size_t limit = nlen - dlen + 1;
	MutableBigInteger q;
	q.value.assign(limit, 0);
	q.offset = 0;
	q.intLen = limit;
	if (rem.intLen == nlen)
	{
		rem.offset = 0;
		rem.value[0] = 0;
		rem.intLen++;
	}
	const int32_t dh = divisor[0];
	const uint32_t dhLong = (uint32_t)dh;
	const uint32_t dl = (uint32_t)divisor[1];
	for (size_t j = 0; j < limit - 1; j++)
	{
		int32_t qhat = 0;
		int32_t qrem = 0;
		bool skipCorrection = false;
		int32_t nh = rem.value[j + rem.offset];
		int32_t nh2 = nh + (int32_t)0x80000000;
		int32_t nm = rem.value[j + 1 + rem.offset];
		if (nh == dh)
		{
			qhat = ~(int32_t)0;
			qrem = nh + nm;
			skipCorrection = (qrem + (int32_t)0x80000000) < nh2;
		}
		else
		{
			int64_t nChunk = (((int64_t)nh) << 32) | (uint32_t)nm;
			if (nChunk >= 0)
			{
				qhat = (int32_t)(nChunk / dhLong);
				qrem = (int32_t)(nChunk - (qhat * dhLong));
			}
			else
			{
				int64_t tmp = divWord(nChunk, dh);
				qhat = (int32_t)tmp;
				qrem = (int32_t)((uint64_t)tmp >> 32);
			}
		}
		if (qhat == 0)
		{
			continue;
		}
		if (!skipCorrection)
		{
			uint32_t nl = (uint32_t)rem.value[j + 2 + rem.offset];
			uint64_t rs = ((uint64_t)(uint32_t)qrem << 32) | nl;
			uint64_t estProduct = (uint64_t)dl * (uint32_t)qhat;
			if (estProduct > rs)
			{
				qhat--;
				qrem = (int32_t)((uint32_t)qrem + dhLong);
				if ((uint32_t)qrem >= dhLong)
				{
					estProduct -= dl;
					rs = ((uint64_t)(uint32_t)qrem << 32) | nl;
					if (estProduct >= rs)
					{
						qhat--;
					}
				}
			}
		}
		rem.value[j + rem.offset] = 0;
		int32_t borrow = mulsub(rem.value, divisor, qhat, dlen, j + rem.offset);
		if ((borrow + (int32_t)0x80000000) > nh2)
		{
			divadd(divisor, rem.value, j + 1 + rem.offset);
			qhat--;
		}
		q.value[j] = qhat;
	}
	int32_t qhat = 0;
	int32_t qrem = 0;
	bool skipCorrection = false;
	int32_t nh = rem.value[limit - 1 + rem.offset];
	int32_t nh2 = nh + (int32_t)0x80000000;
	int32_t nm = rem.value[limit + rem.offset];
	if (nh == dh)
	{
		qhat = ~(int32_t)0;
		qrem = nh + nm;
		skipCorrection = (qrem + (int32_t)0x80000000) < nh2;
	}
	else
	{
		int64_t nChunk = (((int64_t)nh) << 32) | (uint32_t)nm;
		if (nChunk >= 0)
		{
			qhat = (int32_t)(nChunk / dhLong);
			qrem = (int32_t)(nChunk - (qhat * dhLong));
		}
		else
		{
			int64_t tmp = divWord(nChunk, dh);
			qhat = (int32_t)tmp;
			qrem = (int32_t)((uint64_t)tmp >> 32);
		}
	}
	if (qhat != 0)
	{
		if (!skipCorrection)
		{
			uint32_t nl = (uint32_t)rem.value[limit + 1 + rem.offset];
			uint64_t rs = ((uint64_t)(uint32_t)qrem << 32) | nl;
			uint64_t estProduct = (uint64_t)dl * (uint32_t)qhat;
			if (estProduct > rs)
			{
				qhat--;
				qrem = (int32_t)((uint32_t)qrem + dhLong);
				if ((uint32_t)qrem >= dhLong)
				{
					estProduct -= dl;
					rs = ((uint64_t)(uint32_t)qrem << 32) | nl;
					if (estProduct >= rs)
					{
						qhat--;
					}
				}
			}
		}
		int32_t borrow;
		rem.value[limit - 1 + rem.offset] = 0;
		borrow = mulsub(rem.value, divisor, qhat, dlen, limit - 1 + rem.offset);
		if ((borrow + (int32_t)0x80000000) > nh2) {
			divadd(divisor, rem.value, limit - 1 + 1 + rem.offset);
			qhat--;
		}
		q.value[limit - 1] = qhat;
	}
	quotient = q.toBigInteger();
	return rem.toBigInteger() >> shift;
}

BigInteger BigInteger::multiplyByInt(const std::vector<int32_t> x, const uint32_t y, int sign)
{
	if (bitCount(y) == 1)
	{
		return BigInteger(sign, shiftLeft(x, numberOfTrailingZeroes(y)));
	}
	size_t xlen = x.size();
	std::vector<int32_t> rmag(xlen + 1, 0);
	int64_t carry = 0;
	size_t rstart = rmag.size() - 1;
	for (size_t i = xlen; i-- > 0; )
	{
		int64_t product = (int64_t)(uint32_t)x[i] * y + carry;
		rmag[rstart--] = (int32_t)product;
		carry = (uint64_t)product >> 32;
	}
	if (carry == 0)
	{
		rmag.erase(rmag.begin());
	}
	else
	{
		rmag[rstart] = (int32_t)carry;
	}
	return BigInteger(sign, rmag);
}

std::vector<int32_t> BigInteger::multiplyToLen(const std::vector<int32_t> x, size_t xlen, const std::vector<int32_t> y, size_t ylen)
{
	size_t xstart = xlen - 1;
	size_t ystart = ylen;
	std::vector<int32_t> z(xlen + ylen, 0);
	int64_t carry = 0;
	for (size_t j = ystart, k = ystart + 1 + xstart; j-- > 0; )
	{
		int64_t product = (int64_t)(uint32_t)y[j] * (uint32_t)x[xstart] + carry;
		z[--k] = (int32_t)product;
		carry = (uint64_t)product >> 32;
	}
	z[xstart] = (int32_t)carry;
	for (size_t i = xstart; i-- > 0; )
	{
		carry = 0;
		for (size_t j = ystart, k = ystart + 1 + i; j-- > 0; )
		{
			int64_t product = (int64_t)(uint32_t)y[j] * (uint32_t)x[i] + (uint32_t)z[--k] + carry;
			z[k] = (int32_t)product;
			carry = (uint64_t)product >> 32;
		}
		z[i] = (int32_t)carry;
	}
	return z;
}

void BigInteger::copyAndShift(const std::vector<int32_t> & src, size_t srcFrom, size_t srcLen, std::vector<int32_t> & dst, size_t dstFrom, int shift)
{
	int n2 = 32 - shift;
	int32_t c = src[srcFrom];
	for (size_t i = 0; i < srcLen - 1; i++)
	{
		int32_t b = c;
		c = src[++srcFrom];
		dst[dstFrom + i] = (b << shift) | ((uint32_t)c >> n2);
	}
	dst[dstFrom + srcLen - 1] = c << shift;
}

int64_t BigInteger::divWord(int64_t n, int32_t d)
{
	uint32_t dLong = (uint32_t)d;
	int64_t r;
	int64_t q;
	if (dLong == 1)
	{
		q = (int32_t)n;
		r = 0;
		return ((uint64_t)r << 32) | (uint32_t)q;
	}
	q = (int64_t)(((uint64_t)n >> 1) / (dLong >> 1));
	r = n - q * dLong;
	while (r < 0)
	{
		r += dLong;
		q--;
	}
	while (r >= dLong)
	{
		r -= dLong;
		q++;
	}
	return ((uint64_t)r << 32) | q;
}

int32_t BigInteger::mulsub(std::vector<int32_t> & q, std::vector<int32_t> & a, int32_t x, size_t len, size_t offset)
{
	uint32_t xLong = (uint32_t)x;
	int64_t carry = 0;
	offset += len;
	for (size_t j = len; j-- > 0; )
	{
		int64_t product = (uint64_t)(uint32_t)a[j] * xLong + carry;
		int64_t difference = q[offset] - product;
		q[offset--] = (int32_t)difference;
		carry = ((uint64_t)product >> 32) + (((uint32_t)difference) > ((uint64_t)(uint32_t)(~(int32_t)product)) ? 1 : 0);
	}
	return (int32_t)carry;
}

int32_t BigInteger::mulsubBorrow(std::vector<int32_t> & q, std::vector<int32_t> & a, int32_t x, size_t len, size_t offset)
{
	uint32_t xLong = (uint32_t)x;
	int64_t carry = 0;
	offset += len;
	for (size_t j = len; j-- > 0; )
	{
		int64_t product = (uint64_t)(uint32_t)a[j] * xLong + carry;
		int64_t difference = q[offset--] - product;
		carry = ((uint64_t)product >> 32) + (((uint64_t)difference) > (uint64_t)(~(int32_t)product) ? 1 : 0);
	}
	return (int32_t)carry;
}

int32_t BigInteger::divadd(std::vector<int32_t> & a, std::vector<int32_t> & result, size_t offset)
{
	int64_t carry = 0;
	for (auto j = a.size(); j-- > 0; )
	{
		int64_t sum = (uint64_t)(uint32_t)a[j] + (uint32_t)result[j + offset] + carry;
		result[j + offset] = (int32_t)sum;
		carry = (uint64_t)sum >> 32;
	}
	return (int32_t)carry;
}

std::istream & operator>>(std::istream & input, const BigInteger & value)
{
	return input;
}

std::ostream & operator<<(std::ostream & output, const BigInteger & value)
{
	return output << value.toString();
}

void BigInteger::toString(const BigInteger & u, std::string & sb, int radix, size_t digits)
{
	if (u.mag.size() <= SCHOENHAGE_BASE_CONVERSION_THRESHOLD)
	{
		std::string s = u.smallToString(radix);
		if (s.size() < digits && sb.size() > 0)
		{
			for (auto i = s.size(); i < digits; i++)
			{
				sb.append("0");
			}
		}
		sb.append(s);
		return;
	}
	size_t b = u.bitLength();
	int n = (int) std::round(std::log(b * LOG_TWO / LOG_CACHE[radix]) / LOG_TWO - 1.0);
	BigInteger v = getRadixConversionCache(radix, n);
	BigInteger q;
	BigInteger r = u.divide(v, q);
	int expectedDigits = 1 << n;
	toString(q, sb, radix, digits - expectedDigits);
	toString(r, sb, radix, expectedDigits);
}

BigInteger BigInteger::getRadixConversionCache(int radix, int exponent)
{
	std::vector<BigInteger> cacheLine = POWER_CACHE[radix];
	if ((size_t)exponent >= cacheLine.size())
	{
		for (auto i = cacheLine.size(); i <= (size_t)exponent; i++)
		{
			cacheLine[i] = cacheLine[i - 1] << 1; // .pow(2)
		}
	}
	return cacheLine[exponent];
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
		difference = (int64_t)(uint32_t)big[--bigIndex] - (uint32_t)little[--littleIndex] + (difference >> 32);
		result[bigIndex] = (int32_t)difference;
	}
	bool borrow = (difference >> 32) != 0;
	while (bigIndex > 0 && borrow)
	{
		borrow = ((result[--bigIndex] = big[bigIndex] - 1) == -1);
	}
	while (bigIndex > 0)
	{
		result[--bigIndex] = big[bigIndex];
	}
	return result;
}

std::vector<int32_t> BigInteger::shiftLeft(std::vector<int32_t> mag, int32_t n)
{
	size_t nInts = (uint32_t)n >> 5;
	int nBits = n & 0x1F;
	size_t magLen = mag.size();
	std::vector<int32_t> newMag;
	if (nBits == 0)
	{
		newMag.assign(magLen + nInts, 0);
		newMag.insert(newMag.begin(), mag.begin(), mag.end());
	}
	else
	{
		size_t i = 0;
		int nBits2 = 32 - nBits;
		int32_t highBits = (uint32_t)mag[0] >> nBits2;
		if (highBits != 0)
		{
			newMag.assign(magLen + nInts + 1, 0);
			newMag[i++] = highBits;
		}
		else
		{
			newMag.assign(magLen + nInts, 0);
		}
		size_t j = 0;
		while (j < magLen - 1)
		{
			newMag[i++] = mag[j++] << nBits | (uint32_t)mag[j] >> nBits2;
		}
		newMag[i] = mag[j] << nBits;
	}
	return newMag;
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

int BigInteger::bitLengthForInt(int32_t i)
{
	return 32 - numberOfLeadingZeroes(i);
}

int BigInteger::numberOfLeadingZeroes(int32_t i)
{
	if (i == 0)
	{
		return 32;
	}
	int n = 1;
	if ((uint32_t)i >> 16 == 0) { n += 16; i <<= 16; }
	if ((uint32_t)i >> 24 == 0) { n += 8; i <<= 8; }
	if ((uint32_t)i >> 28 == 0) { n += 4; i <<= 4; }
	if ((uint32_t)i >> 30 == 0) { n += 2; i <<= 2; }
	n -= (uint32_t)i >> 31;
	return n;
}

int BigInteger::numberOfTrailingZeroes(int32_t i)
{
	int y;
	if (i == 0)
	{
		return 32;
	}
	int n = 31;
	y = i << 16; if (y != 0) { n = n - 16; i = y; }
	y = i << 8; if (y != 0) { n = n - 8; i = y; }
	y = i << 4; if (y != 0) { n = n - 4; i = y; }
	y = i << 2; if (y != 0) { n = n - 2; i = y; }
	return n - ((uint32_t)(i << 1) >> 31);
}

int BigInteger::bitCount(int32_t i)
{
	i = i - (((uint32_t)i >> 1) & 0x55555555);
	i = (i & 0x33333333) + (((uint32_t)i >> 2) & 0x33333333);
	i = (i + ((uint32_t)i >> 4)) & 0x0F0F0F0F;
	i = i + ((uint32_t)i >> 8);
	i = i + ((uint32_t)i >> 16);
	return i & 0x3F;
}

std::string BigInteger::lltoa(int64_t i, int radix)
{
	static const char digits[] = {
		'0' , '1' , '2' , '3' , '4' , '5' ,
		'6' , '7' , '8' , '9' , 'a' , 'b' ,
		'c' , 'd' , 'e' , 'f' , 'g' , 'h' ,
		'i' , 'j' , 'k' , 'l' , 'm' , 'n' ,
		'o' , 'p' , 'q' , 'r' , 's' , 't' ,
		'u' , 'v' , 'w' , 'x' , 'y' , 'z'
	};
	if (radix < MIN_RADIX || radix > MAX_RADIX)
	{
		radix = 10;
	}
	std::string buf;
	bool negative = i < 0;
	if (!negative)
	{
		i = -i;
	}
	while (i <= -radix)
	{
		buf.insert(buf.begin(), digits[-(i % radix)]);
		i /= radix;
	}
	buf.insert(buf.begin(), digits[-i]);
	if (negative)
	{
		buf.insert(buf.begin(), '-');
	}
	return buf;
}
