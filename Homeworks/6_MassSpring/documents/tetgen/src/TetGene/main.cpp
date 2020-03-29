#pragma once
#include "Tet_generate.h"
#include "tetgen.h"

#include<iostream>

using namespace std;

int main()
{
	tetgenio  *a;
	tetgenio  *c;
	CTet_generate  b;
	a = b.poly_in("../data/ditto_02.stl");
	c = b.tet_out(a);
	cout << c->numberofpoints << endl;
	b.tet_output(c, "../data/ditto_02.tet");
	return 0;
}