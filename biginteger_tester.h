#pragma once

#include <functional>

#include "biginteger.h"

struct MenuItem
{
	std::string name;

	std::function<bool(BigInteger &, BigInteger &)> action;
};

template <typename T>
std::function<bool(BigInteger &, BigInteger &)> printing(T(BigInteger::*operation)(const BigInteger &) const, std::string symbol);
