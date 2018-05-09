#include <cstdarg>

#include "stdafx.h"
#include "biginteger.h"

#define CLEAR(in) { \
		in.clear(); \
		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }

void getBigInteger(std::istream & in, BigInteger & c, std::string name)
{
	while (std::cin)
	{
		std::cout << "Enter value for big integer " << name << ": ";
		if (std::cin >> c)
		{
			break;
		}
		CLEAR(in);
	}
}

typedef bool BiFunction(BigInteger & a, BigInteger & b);

struct MenuItem
{
	std::string name;

	BiFunction * action;
};

BiFunction * printing(BigInteger operation(BigInteger & a, BigInteger & b))
{
	return [](BigInteger & a, BigInteger & b)
	{
		BigInteger & r = operation(a, b);
		
	};
}

int main(void)
{
	BigInteger a, b;
	getBigInteger(std::cin, a, "A");
	getBigInteger(std::cin, b, "B");
	std::vector<MenuItem> menu =
	{
		{ "Less than", [](auto a, auto b) { return a < b; } },
		{ "Greater than", [](auto a, auto b) { return a > b; } }
		{ "Equality", [](auto a, auto b) { return a == b; } }
		{ "Addition", [](auto a, auto b) { return a + b; } }
		{ "Subtraction", [](auto a, auto b) { return a - b; } }
		{ "Multiplication", [](auto a, auto b) { return a * b; } }
		{ "Division", [](auto a, auto b) { return a / b; } }
		{ "Modulus", [](auto a, auto b) { return a % b; } }
		{ "Exit", [](auto a, auto b) { return a < b; } }
	};
	while (std::cin)
	{
		// std::cout menu
		int selection;
		std::cin >> selection;
		if (std::cin.eof())
		{
			break;
		}
		switch (selection)
		{
		case 1:
		case 1:
			std::cout << "(" << a << ") + (" << b << ") = " << (a + b) << std::endl;
			break;
		case 2:
			std::cout << "(" << a << ") - (" << b << ") = " << (a - b) << std::endl;
			break;
		case 3:
			std::cout << "(" << a << ") * (" << b << ") = " << (a * b) << std::endl;
			break;
		case 4:
			return EXIT_SUCCESS;
		default:
			std::cout << "Unknown operation" << std::endl;
			CLEAR(std::cin);		
		}
	}
	return EXIT_FAILURE;
}
