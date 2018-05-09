/*
* Paul Fulham
*/

#include <cstdlib>
#include <limits>
#include <stdio.h>

#include "biginteger.h"
#include "biginteger_tester.h"

template <typename T>
std::function<bool(BigInteger &, BigInteger &)> printing(T(BigInteger::*operation)(const BigInteger &) const, std::string symbol)
{
	return [=](BigInteger & a, BigInteger & b) -> bool
	{
		std::cout << a << " " << symbol << " " << b << " = " << (a.*operation)(b) << std::endl;
		return false;
	};
}

void clear(std::istream & in)
{
	in.clear();
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

BigInteger prompt(std::istream & in, std::string name)
{
	while (std::cin)
	{
		std::cout << "Enter value for big integer " << name << ": ";
		BigInteger c;
		if (std::cin >> c)
		{
			return c;
		}
		clear(in);
	}
	return 0;
}

int main()
{
	BigInteger a = prompt(std::cin, "A");
	BigInteger b = prompt(std::cin, "B");
	std::vector<MenuItem> menu
	{
		{ "Less than", printing(&BigInteger::operator<, "<") },
		{ "Greater than", printing(&BigInteger::operator>, ">") },
		{ "Equality", printing(&BigInteger::operator==, "==") },
		{ "Addition", printing(&BigInteger::operator+, "+") },
		{ "Subtraction", printing(&BigInteger::operator-, "-") },
		{ "Multiplication", printing(&BigInteger::operator*, "*") },
		{ "Division", printing(&BigInteger::operator/,"/") },
		{ "Modulus", printing(&BigInteger::operator%, "%") },
		{ "Exit", [](auto a, auto b) { return true; } }
	};
	int status = EXIT_FAILURE;
	while (std::cin)
	{
		int num = 1;
		for (auto item : menu)
		{
			std::cout << (num++) << ". " << item.name << std::endl;
		}
		size_t selection = 0;
		if ((std::cin >> selection).eof())
		{
			break;
		}
		if (selection >= 1 && selection <= menu.size())
		{
			if (menu[selection - 1].action(a, b))
			{
				status = EXIT_SUCCESS;
				break;
			}
		}
		else
		{
			std::cout << "Unknown operation" << std::endl;
			clear(std::cin);
		}
	}
	return status;
}
