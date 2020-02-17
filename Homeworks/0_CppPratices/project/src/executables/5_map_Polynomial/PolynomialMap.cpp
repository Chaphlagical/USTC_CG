#include "PolynomialMap.h"

void PolynomialMap::AddOneTerm(int deg, double cof)
{
	if (m_Polynomial.find(deg) == m_Polynomial.end())
	{
		m_Polynomial.insert(std::map<int, double>::value_type(deg, cof));
	}
	else
	{
		std::map<int, double>::iterator it = m_Polynomial.find(deg);
		it->second += cof;
	}
}

void PolynomialMap::ReadFromFile(std::string file)
{
	std::ifstream infile;
	char buf;
	int n, cof;
	double deg;

	infile.open(file.c_str(), std::ios::in);
	infile >> buf;
	infile >> n;

	//m_Polynomial.insert(std::map<int, double>::value_type(1,1));
	for (int i = 0; i < n; i++)
	{
		infile >> deg;
		infile >> cof;
		AddOneTerm(deg, cof);
	}
}

PolynomialMap::PolynomialMap()
{

}

PolynomialMap::PolynomialMap(std::string file)
{
	ReadFromFile(file);
}

PolynomialMap::PolynomialMap(double *cof, double *deg, int n)
{
	for (int i = 0; i < n; i++)
	{
		AddOneTerm(deg[i], cof[i]);
	}
}

PolynomialMap::PolynomialMap(std::vector<double>cof, std::vector<int>deg)
{
	if (cof.size() != deg.size())
	{
		std::cout << "(cof.size() != deg.size()" << std::endl;
	}

	size_t size = cof.size() < deg.size() ? cof.size() : deg.size();

	for (int i = 0; i < size; i++)
	{
		AddOneTerm(deg[i], cof[i]);
	}
}

PolynomialMap::~PolynomialMap()
{

}

void PolynomialMap::Print()
{
	std::map<int, double>::iterator it = m_Polynomial.begin();

	if (it == m_Polynomial.end())
	{
		std::cout << "PolynomialMapMap empty!" << std::endl;
		return;
	}

	if (it->first == 0)
		std::cout << it->second;
	else
		std::cout << it->second << "*x^" << it->first;
	it++;

	for (; it != m_Polynomial.end(); it++)
	{
		if (it->second > 0)
			std::cout << "+" << it->second << "*x^" << it->first;
		else if (it->second < 0)
			std::cout << it->second << "*x^" << it->first;
	}
	std::cout << std::endl;
}

PolynomialMap PolynomialMap::operator+(const PolynomialMap &right)
{
	PolynomialMap mp(*this);
	std::map<int, double>::const_iterator it = right.m_Polynomial.begin();

	for (; it != right.m_Polynomial.end(); it++)
	{
		if (mp.m_Polynomial.find(it->first) == mp.m_Polynomial.end())
		{
			mp.m_Polynomial.insert(std::map<int, double>::value_type(it->first, it->second));
		}
		else
		{
			mp.m_Polynomial.find(it->first)->second += it->second;
		}
	}

	return mp;
}


PolynomialMap PolynomialMap::operator-(const PolynomialMap &right)
{
	PolynomialMap mp(*this);
	std::map<int, double>::const_iterator it = right.m_Polynomial.begin();

	for (; it != right.m_Polynomial.end(); it++)
	{
		if (mp.m_Polynomial.find(it->first) == mp.m_Polynomial.end())
		{
			mp.m_Polynomial.insert(std::map<int, double>::value_type(it->first, it->second));
		}
		else
		{
			mp.m_Polynomial.find(it->first)->second -= it->second;
		}
	}

	return mp;
}

PolynomialMap PolynomialMap::operator*(const PolynomialMap &right)
{
	PolynomialMap mp;

	for (std::map<int, double>::const_iterator it_left = right.m_Polynomial.begin(); it_left != right.m_Polynomial.end(); it_left++)
		for (std::map<int, double>::const_iterator it_right = m_Polynomial.begin(); it_right != m_Polynomial.end(); it_right++)
		{
			mp.AddOneTerm(it_left->first + it_right->first, it_left->second*it_right->second);
		}

	return mp;
}

PolynomialMap& PolynomialMap::operator=(const PolynomialMap &right)
{
	m_Polynomial.clear();
	std::map<int, double>::const_iterator it = right.m_Polynomial.begin();
	for (; it != right.m_Polynomial.end(); it++)
	{
		AddOneTerm(it->first, it->second);
	}
	return *this;
}