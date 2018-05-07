#include "stdafx.h"
#include "biginteger.h"

int main()
{
	BigInteger a(8290000440005840200);
	BigInteger b(8999999999994020032);
	std::cout << (a + b) << std::endl;
	std::cout << (a - b) << std::endl;
	std::cout << (a * b) << std::endl;
    return EXIT_SUCCESS;
}
