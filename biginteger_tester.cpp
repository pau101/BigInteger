#include "stdafx.h"
#include "biginteger.h"

int main()
{
	BigInteger a(8290000440005840200);
	BigInteger b(809999994020032);
	std::cout << (a + a) << std::endl;
	std::cout << (a - b) << std::endl;
	std::cout << (a * b * b) << std::endl;
	std::cout << (a / b) << std::endl;
    return EXIT_SUCCESS;
}
