#include "PolynomialList.h"

bool Node_cmp(const Node &node1, const Node &node2)
{
	return node1.deg < node2.deg ? true : false;
}

void PolynomialList::Compress()
{
	std::list<Node>::iterator it1, it2, it3;
	it1 = m_Polynomial.begin();
	for (; it1 != m_Polynomial.end(); it1++)
	{
		for (it2 = it1; ++it2 != m_Polynomial.end();)
		{
			if (it1->deg == it2->deg)
			{
				it1->cof += it2->cof;
				it2 = --m_Polynomial.erase(it2);
			}
		}
	}

	it1 = m_Polynomial.begin();
	for (; it1 != m_Polynomial.end(); it1++)
	{
		if (it1->cof == 0)
			it1 = --m_Polynomial.erase(it1);
	}
}

void PolynomialList::ReadFromFile(std::string file)
{
	std::ifstream infile;
	char buf;
	int n;

	infile.open(file.c_str(), std::ios::in);
	infile >> buf;
	infile >> n;

	for (int i = 0; i < n; i++)
	{
		Node node;
		infile >> node.deg;
		infile >> node.cof;
		m_Polynomial.push_back(node);
	}

}

void PolynomialList::AddOneTerm(Node term)
{
	m_Polynomial.push_back(term);
}

PolynomialList::PolynomialList()
{

}

PolynomialList::PolynomialList(std::string file)
{
	ReadFromFile(file);
	m_Polynomial.sort(Node_cmp);
	Compress();
}

PolynomialList::PolynomialList(double *cof, double *deg, int n)
{
	for (int i = 0; i < n; i++)
	{
		Node node;
		node.cof = cof[i];
		node.deg = deg[i];
		AddOneTerm(node);
	}
	m_Polynomial.sort(Node_cmp);
	Compress();
}

PolynomialList::PolynomialList(std::vector<double>cof, std::vector<int>deg)
{
	if (cof.size() != deg.size())
	{
		std::cout << "(cof.size() != deg.size()" << std::endl;
	}

	size_t size = cof.size() < deg.size() ? cof.size() : deg.size();

	for (int i = 0; i < size; i++)
	{
		Node node = { cof[i],deg[i] };
		AddOneTerm(node);
	}
	m_Polynomial.sort(Node_cmp);
	Compress();
}

PolynomialList::~PolynomialList()
{

}

void PolynomialList::Print()
{
	std::list<Node>::iterator it = m_Polynomial.begin();

	if (it == m_Polynomial.end())
	{
		std::cout << "Polynomial empty!" << std::endl;
		return;
	}

	if (it->deg == 0)
	{
		std::cout << it->cof;
	}
	else
	{
		std::cout << it->cof << "*x^" << it->deg;
	}
	it++;

	for (; it != m_Polynomial.end(); it++)
	{
		if (it->cof >= 0)
			std::cout << "+" << it->cof << "*x^" << it->deg;
		else
			std::cout << it->cof << "*x^" << it->deg;
	}
	std::cout << std::endl;
}

PolynomialList &PolynomialList::operator=(const PolynomialList &right)
{
	m_Polynomial.clear();
	std::list<Node>::const_iterator it = right.m_Polynomial.begin();
	for (; it != right.m_Polynomial.end(); it++)
	{
		AddOneTerm(*it);
	}
	return *this;
}

PolynomialList PolynomialList::operator+(const PolynomialList &right)
{
	PolynomialList ploy(right);
	std::list<Node>::const_iterator it = m_Polynomial.begin();

	for (; it != m_Polynomial.end(); it++)
	{
		ploy.AddOneTerm(*it);
	}

	ploy.m_Polynomial.sort(Node_cmp);
	ploy.Compress();
	return ploy;
}

PolynomialList PolynomialList::operator-(const PolynomialList &right)
{
	PolynomialList ploy(*this);
	std::list<Node>::const_iterator it = right.m_Polynomial.begin();

	for (; it != right.m_Polynomial.end(); it++)
	{
		Node node = { -it->cof ,it->deg };
		ploy.AddOneTerm(node);
	}

	ploy.m_Polynomial.sort(Node_cmp);
	ploy.Compress();
	return ploy;
}

PolynomialList PolynomialList::operator*(const PolynomialList &right)
{
	PolynomialList ploy;
	int count = 0;
	for (std::list<Node>::const_iterator it_right = right.m_Polynomial.begin(); it_right != right.m_Polynomial.end(); it_right++)
	{
		for (std::list<Node>::const_iterator it_left = m_Polynomial.begin(); it_left != m_Polynomial.end(); it_left++)
		{
			Node node = { it_right->cof*it_left->cof, it_right->deg + it_left->deg };
			ploy.AddOneTerm(node);
		}
	}

	ploy.m_Polynomial.sort(Node_cmp);
	ploy.Compress();
	return ploy;
}