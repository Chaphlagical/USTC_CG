#include <string>

#include "TestPolynomial.h"
#include <cstdlib>
#include <ctime>

using namespace std;

int main()
{
    srand(unsigned(time(0)));
	std::vector<int> deg0, deg1;
	std::vector<double> cof0, cof1;
	PolynomialTest Poly;

	Poly.generateTestCase(1000, deg0, cof0);
	Poly.generateTestCase(1000, deg1, cof1);

	Poly.testOperation(deg0, cof0, deg1, cof1);

    return 0;
}


