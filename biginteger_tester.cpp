#include "stdafx.h"
#include "biginteger.h"

int main()
{
	BigInteger a;
	BigInteger b;
	std::cout << (BigInteger(25) / BigInteger(5) == 5) << std::endl;
	std::cout << (BigInteger(250000000) / BigInteger(48) == 5208333) << std::endl;
	std::cout << (BigInteger(59) / BigInteger(25) == 2) << std::endl;
	std::cout << (BigInteger("5894082498674691308000005") / BigInteger(38594) == BigInteger("152720065391185036741")) << std::endl;
	/*std::cout << "A: ";
	std::cout.flush();
	std::cin >> a;
	std::cout << "B: ";
	std::cout.flush();
	std::cin >> b;
	std::cout << "A, B: " << a << ", " << b << std::endl;
	std::cout << (a + a) << std::endl;
	std::cout << (a - b) << std::endl;
	std::cout << (a * b * b) << std::endl;
	std::cout << (a / b) << std::endl;*/
    return EXIT_SUCCESS;
}
