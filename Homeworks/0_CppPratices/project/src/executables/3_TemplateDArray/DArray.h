# include <iostream>

#define SAFEDELETE(p)	if(p)	{delete p;	p=NULL;}
#define SAFEDELETES(p)		if(p)	{delete [] p;	p=NULL;}

template <class DataType>
class DArray
{
private:
	DataType *m_pData;	//存放数组动态内存指针
	int m_nSize;				//数组的元素个数

private:
	void Init();	//初始化
	void Free();	//释放动态内存
	inline int InvalidateIndex(int nIndex);	//判断下标的合法性

public:
	DArray();	//缺省构造函数
	DArray(int nSize, DataType dValue = 0);	//其他构造函数，设置一定数组大小，并设置所有元素为0
	DArray(const DArray& arr);	// 拷贝构造函数（最好为所有包含动态分配成员的类都提供拷贝构造函数）
	~DArray();	//析构函数
	void Print();	//输出显示所有数组元素的值
	int GetSize();	//获取数组的大小
	int SetSize(int nSize);	//重新设置数组的大小（若nSize小于原数组的大小，截取前nSize个元素作为新数组的元素；若nSize大于原数组的大小，新增的元素的值设置缺省值0）
	DataType GetAt(int nIndex);	//获取某个元素
	DataType operator[](int nIndex) const;	//重载[]操作符，以便像传统数组那样通过a[k]来获取元素的值
	int SetAt(int nIndex, DataType dValue);	//设置某个元素的值
	int PushBack(DataType dValue);	//追加一个新的元素到数组末尾
	int DeleteAt(int nIndex);	//从数组中删除一个元素
	int InsertAt(int nIndex, DataType dValue);	//插入一个新的元素到数组中
	DArray 	operator = (const DArray& array);  	// 重载赋值操作符号"="
};


template <class DataType>
void DArray<DataType>::Init()
{
	m_nSize = 0;
	m_pData = NULL;
}

template <class DataType>
void DArray<DataType>::Free()
{
	SAFEDELETES(m_pData);
	m_nSize = 0;
}

template <class DataType>
inline int DArray<DataType>::InvalidateIndex(int nIndex)
{
	if (nIndex < 0 || nIndex > m_nSize)
		return 0;
	else
		return 1;
}

template <class DataType>
DArray<DataType>::DArray()
{
	Init();
}

template <class DataType>
DArray<DataType>::DArray(const DArray& arr)
{
	m_nSize = arr.m_nSize;
	m_pData = new double[m_nSize];
	for (int i = 0; i < m_nSize; i++)
	{
		m_pData[i] = arr[i];
	}
}

template <class DataType>
DArray<DataType>::~DArray()
{
	Free();
}

template <class DataType>
DArray<DataType>::DArray(int nSize, DataType dValue)
{
	m_nSize = nSize;
	m_pData = new double[m_nSize];

	for (int i = 0; i < nSize; i++)
		m_pData[i] = dValue;
}

template <class DataType>
void DArray<DataType>::Print()
{
	std::cout << "size=" << m_nSize << " ";
	for (int i = 0; i < m_nSize; i++)
	{
		std::cout << m_pData[i] << ' ';
	}
	std::cout << std::endl;
}

template <class DataType>
int DArray<DataType>::GetSize()
{
	return m_nSize;
}

template <class DataType>
int DArray<DataType>::SetSize(int nSize)
{
	DataType *pt;
	pt = new DataType[nSize];

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
	return 0;
}

template <class DataType>
DataType DArray<DataType>::GetAt(int nIndex)
{
	if (InvalidateIndex(nIndex))
		return m_pData[nIndex];
	else
	{
		std::cout << "Index Invaildable!" << std::endl;
		return 0;
	}
}

template <class DataType>
DataType DArray<DataType>::operator[](int nIndex) const
{
	return m_pData[nIndex];
}

template <class DataType>
int DArray<DataType>::SetAt(int nIndex, DataType dValue)
{
	if (InvalidateIndex(nIndex))
	{
		m_pData[nIndex] = dValue;
		return 1;
	}
	else
	{
		std::cout << "Index Invaildable!" << std::endl;
		return 0;
	}
}

template <class DataType>
int DArray<DataType>::PushBack(DataType dValue)
{
	DataType *pt;
	pt = new DataType[m_nSize + 1];

	for (int i = 0; i < m_nSize; i++)
		pt[i] = m_pData[i];

	pt[m_nSize] = dValue;
	delete[] m_pData;
	m_nSize += 1;
	m_pData = pt;

	return 1;
}

template <class DataType>
int DArray<DataType>::DeleteAt(int nIndex)
{
	if (InvalidateIndex(nIndex))
	{
		DataType *pt;
		pt = new DataType[m_nSize - 1];

		for (int i = 0; i < nIndex; i++)
			pt[i] = m_pData[i];

		for (int i = nIndex + 1; i < m_nSize; i++)
			pt[i - 1] = m_pData[i];

		delete[] m_pData;
		m_nSize -= 1;
		m_pData = pt;
	}
	else
	{
		std::cout << "Index Invaildable!" << std::endl;
		return 0;
	}
}

template <class DataType>
int DArray<DataType>::InsertAt(int nIndex, DataType dValue)
{
	if (InvalidateIndex(nIndex))
	{
		DataType *pt;

		pt = new DataType[m_nSize + 1];
		for (int i = 0; i < nIndex; i++)
			pt[i] = m_pData[i];

		pt[nIndex] = dValue;
		for (int i = nIndex + 1; i < m_nSize + 1; i++)
			pt[i] = m_pData[i - 1];

		m_nSize += 1;
		delete[] m_pData;
		m_pData = pt;
	}
	else
	{
		std::cout << "Index Invaildable!" << std::endl;
		return 0;
	}
}

template <class DataType>
DArray<DataType> 	DArray<DataType>::operator = (const DArray& array) {
	m_nSize = array.m_nSize;
	m_pData = new double[array.m_nSize];
	for (int i = 0; i < array.m_nSize; i++)
	{
		m_pData[i] = array.m_pData[i];
	}
	return *this;
}