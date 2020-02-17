#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <vector>

class PolynomialMap
{
private:
	std::map<int, double> m_Polynomial;

public:
	PolynomialMap();
	PolynomialMap(std::string file);
	PolynomialMap(double *cof, double *deg, int n);
	PolynomialMap(std::vector<double>cof, std::vector<int>deg);
	virtual ~PolynomialMap();

	PolynomialMap operator+(const PolynomialMap &right);
	PolynomialMap operator-(const PolynomialMap &right);
	PolynomialMap operator*(const PolynomialMap &right);
	PolynomialMap& operator=(const PolynomialMap &right);

	void Print();

private:
	void ReadFromFile(std::string file);
	void AddOneTerm(int deg, double cof);
};