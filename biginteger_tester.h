/*
* Paul Fulham
*
* g++ -o biginteger biginteger.h biginteger.cpp biginteger_tester.h biginteger_tester.cpp
*
* cl /std:c++17 /permissive- /GS -Fe:biginteger.exe -EHsc biginteger.cpp biginteger_tester.cpp
*
* BigInteger is my C++ partial translation of Java's BigInteger
*/

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
