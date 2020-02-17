# include <iostream>

#define SAFEDELETE(p)	if(p)	{delete p;	p=NULL;}
#define SAFEDELETES(p)		if(p)	{delete [] p;	p=NULL;}

template <class DataType>
class DArray
{
private:
	DataType *m_pData;	//������鶯̬�ڴ�ָ��
	int m_nSize;				//�����Ԫ�ظ���

private:
	void Init();	//��ʼ��
	void Free();	//�ͷŶ�̬�ڴ�
	inline int InvalidateIndex(int nIndex);	//�ж��±�ĺϷ���

public:
	DArray();	//ȱʡ���캯��
	DArray(int nSize, DataType dValue = 0);	//�������캯��������һ�������С������������Ԫ��Ϊ0
	DArray(const DArray& arr);	// �������캯�������Ϊ���а�����̬�����Ա���඼�ṩ�������캯����
	~DArray();	//��������
	void Print();	//�����ʾ��������Ԫ�ص�ֵ
	int GetSize();	//��ȡ����Ĵ�С
	int SetSize(int nSize);	//������������Ĵ�С����nSizeС��ԭ����Ĵ�С����ȡǰnSize��Ԫ����Ϊ�������Ԫ�أ���nSize����ԭ����Ĵ�С��������Ԫ�ص�ֵ����ȱʡֵ0��
	DataType GetAt(int nIndex);	//��ȡĳ��Ԫ��
	DataType operator[](int nIndex) const;	//����[]���������Ա���ͳ��������ͨ��a[k]����ȡԪ�ص�ֵ
	int SetAt(int nIndex, DataType dValue);	//����ĳ��Ԫ�ص�ֵ
	int PushBack(DataType dValue);	//׷��һ���µ�Ԫ�ص�����ĩβ
	int DeleteAt(int nIndex);	//��������ɾ��һ��Ԫ��
	int InsertAt(int nIndex, DataType dValue);	//����һ���µ�Ԫ�ص�������
	DArray 	operator = (const DArray& array);  	// ���ظ�ֵ��������"="
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