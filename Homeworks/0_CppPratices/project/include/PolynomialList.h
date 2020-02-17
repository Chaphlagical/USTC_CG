#ifndef POLYNOMIALLIST_H
#define POLYNOMIALLIST_H
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <fstream>

typedef struct Node
{
	double cof;	//coefficient
	int deg;	//degree
}Node;

class PolynomialList
{
private:
	std::list<Node> m_Polynomial;

public:
	PolynomialList();
	PolynomialList(std::string file);
	PolynomialList(double *cof, double *deg, int n);
	PolynomialList(std::vector<double>cof, std::vector<int>deg);
	virtual ~PolynomialList();

	PolynomialList operator+(const PolynomialList &right);
	PolynomialList operator-(const PolynomialList &right);
	PolynomialList operator*(const PolynomialList &right);
	PolynomialList& operator=(const PolynomialList &right);

	void Print();

private:
	void ReadFromFile(std::string file);
	void AddOneTerm(Node term);
	void Compress();
};

#endif