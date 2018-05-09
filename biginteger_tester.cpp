#include "stdafx.h"
#include "biginteger.h"

int main()
{
	BigInteger a;
	BigInteger b;
	std::cout << "A: ";
	std::cout.flush();
	std::cin >> a;
	std::cout << "B: ";
	std::cout.flush();
	std::cin >> b;
	std::cout << "A, B: " << a << ", " << b << std::endl;
	std::cout << (a + a) << std::endl;
	std::cout << (a - b) << std::endl;
	std::cout << (a * b * b) << std::endl;
	std::cout << (a / b) << std::endl;
    return EXIT_SUCCESS;
}
