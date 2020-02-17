#include "DArray.h"

int main(int argc, char** argv)
{
	DArray a;
	a.InsertAt(0, 2.1);
	a.Print();

	a.PushBack(3.0);
	a.PushBack(3.1);
	a.PushBack(3.2);
	a.Print();

	a.DeleteAt(0);
	a.Print();
	a.InsertAt(0, 4.1);
	a.Print();

	DArray  acopy = a;  //�˴��õ��˿������캯��
	acopy.Print();

	DArray  acopy2 (a);  //������ͬ���������䣬���ǳ�ʼ��
	acopy2.Print();

	DArray  acopy3, acopy4; 
	acopy4 = acopy3 = a;	//�˴��õ��˸�ֵ��������"="������
	acopy3.Print();
	acopy4.Print();

	DArray  b;
	b.PushBack(21);
	b.Print();
	b.DeleteAt(0);
	b.Print();
	b.PushBack(22);
	b.SetSize( 5 );
	b.Print();

	DArray  c;
	c.PushBack('a');
	c.PushBack('b');
	c.PushBack('c');
	c.InsertAt(0, 'd');
	c.Print();

	return 0;
}