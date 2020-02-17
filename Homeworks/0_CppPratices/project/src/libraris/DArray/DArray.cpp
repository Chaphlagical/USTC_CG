// implementation of class DArray
#include <iostream>

#include "DArray.h"

using namespace std;

// default constructor
DArray::DArray()
{
	Init();
}

// set an array with default values
DArray::DArray(int nSize, double dValue)
{
	m_nSize = nSize;
	m_nMax = nSize;
	m_pData = new double[m_nSize];

	for (int i = 0; i < nSize; i++)
		m_pData[i] = dValue;
}

DArray::DArray(const DArray& arr)
{
	m_nSize = arr.m_nSize;
	m_nMax = arr.m_nSize;
	m_pData = new double[m_nSize];

	for (int i = 0; i < m_nSize; i++)
	{
		m_pData[i] = arr[i];
	}
}

// deconstructor
DArray::~DArray()
{
	Free();
}

// display the elements of the array
void DArray::Print()
{
	std::cout << "size=" << m_nSize << " ";
	for (int i = 0; i < m_nSize; i++)
	{
		std::cout << m_pData[i] << ' ';
	}
	std::cout << std::endl;
}

// initilize the array
void DArray::Init()
{
	m_nSize = 0;
	m_nMax = 0;
	m_pData = NULL;
}

// free the array
void DArray::Free()
{
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}

	m_nSize = 0;
	m_nMax = 0;
}

// get the size of the array
int DArray::GetSize()
{
	return m_nSize;
}

// set the size of the array
bool DArray::SetSize(int nSize)
{
	double *pt;
	pt = new double[nSize];

	if (nSize > m_nSize)
	{
		for (int i = 0; i < m_nSize; i++)
		{
			pt[i] = m_pData[i];
		}
		for (int i = m_nSize; i < nSize; i++)
		{
			pt[i] = 0;
		}
	}
	else
	{
		for (int i = 0; i <= nSize; i++)
		{
			pt[i] = m_pData[i];
		}
	}
	delete[] m_pData;
	m_pData = pt;
	m_nSize = nSize;
	m_nMax = nSize;
	return 0;
}

inline int DArray::validateIndex(int nIndex)
{
	if (nIndex < 0 || nIndex > m_nSize)
		return 0;
	else
		return 1;
}

// get an element at an index
double DArray::GetAt(int nIndex)
{
	if (validateIndex(nIndex))
		return m_pData[nIndex];
	else
	{
		std::cout << "Index Invaildable!" << std::endl;
		return false;
	}
}

// set the value of an element 
bool	DArray::SetAt(int nIndex, double dValue)
{
	if (validateIndex(nIndex))
	{
		m_pData[nIndex] = dValue;
		return true;
	}
	else
	{
		std::cout << "Index Invaildable!" << std::endl;
		return false;
	}
}

// overload operator '[]'
double DArray::operator[](int nIndex) const
{
	return m_pData[nIndex];
}

// add a new element at the end of the array
bool DArray::PushBack(double dValue)
{
	double *pt;

	if (m_nSize < m_nMax)
	{
		m_pData[m_nSize] = dValue;
		m_nSize++;
	}
	else
	{
		if (m_nMax == 0)
		{
			m_nMax = 1;
		}
		else
		{
			m_nMax *= 2;
		}
		pt = new double[m_nMax];

		for (int i = 0; i < m_nSize; i++)
			pt[i] = m_pData[i];

		pt[m_nSize] = dValue;
		delete[] m_pData;
		m_nSize++;
		m_pData = pt;
	}

	return true;
}

// delete an element at some index
bool DArray::DeleteAt(int nIndex)
{
	if (validateIndex(nIndex))
	{
		double *pt;
		pt = new double[m_nSize - 1];

		for (int i = 0; i < nIndex; i++)
			pt[i] = m_pData[i];

		for (int i = nIndex + 1; i < m_nSize; i++)
			pt[i - 1] = m_pData[i];

		delete[] m_pData;
		m_nSize--;
		m_nMax = m_nSize;
		m_pData = pt;

		return true;
	}
	else
	{
		std::cout << "Index Invaildable!" << std::endl;
		return false;
	}
}

// insert a new element at some index
bool DArray::InsertAt(int nIndex, double dValue)
{
	if (validateIndex(nIndex))
	{
		if (m_nSize < m_nMax)
		{
			for (int i = m_nSize; i > nIndex; i--)
			{
				m_pData[i] = m_pData[i - 1];
			}
			m_pData[nIndex] = dValue;
			m_nSize++;
		}
		else
		{
			double *pt = NULL;

			if (m_nMax == 0)
			{
				m_nMax = 1;
			}
			else
			{
				m_nMax *= 2;
			}

			pt = new double[m_nMax];
			for (int i = 0; i < nIndex; i++)
				pt[i] = m_pData[i];

			pt[nIndex] = dValue;
			for (int i = nIndex + 1; i < m_nSize + 1; i++)
				pt[i] = m_pData[i - 1];

			m_nSize++;
			delete[] m_pData;
			m_pData = pt;
			return true;
		}
	}
	else
	{
		std::cout << "Index Invaildable!" << std::endl;
		return false;
	}
}

// overload operator '='
DArray& DArray::operator = (const DArray& arr)
{
	m_nSize = arr.m_nSize;
	m_pData = new double[arr.m_nSize];
	for (int i = 0; i < arr.m_nSize; i++)
	{
		m_pData[i] = arr.m_pData[i];
	}
	return *this;
}
