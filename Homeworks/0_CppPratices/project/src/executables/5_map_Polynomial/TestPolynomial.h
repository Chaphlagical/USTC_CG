#ifndef TESTPOLYNOMIAL_H
#define TESTPOLYNOMIAL_H
#include <iostream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include "PolynomialList.h"
#include "PolynomialMap.h"

class PolynomialTest
{
public:
	bool generateTestCase(int size, std::vector<int>&deg, std::vector<double>&cof)
	{
		deg.clear();
		cof.clear();
		for (int i = 0; i < size; i++)
		{
			deg.push_back(std::rand() % 10000);
			cof.push_back(std::rand() % 100);
		}
		return true;
	}

	bool testOperation(std::vector<int>&deg1, std::vector<double>&cof1, std::vector<int>&deg2, std::vector<double>&cof2)
	{
		clock_t t0 = clock();
		
		PolynomialList P_list1(cof1, deg1);
		std::cout << "Time for List Polynomial 1 Construction: " << clock() -t0<< "ms" << std::endl;
		t0 = clock();

		PolynomialList P_list2(cof2, deg2);
		std::cout << "Time for List Polynomial 2 Construction: " << clock() - t0<< "ms" << std::endl;
		t0 = clock();

		PolynomialMap P_map1(cof1, deg1);
		std::cout << "Time for Map Polynomial 1 Construction: " << clock() - t0 << "ms" << std::endl;
		t0 = clock();

		PolynomialMap P_map2(cof2, deg2);
		std::cout << "Time for Map Polynomial 2 Construction: " << clock() - t0 << "ms" << std::endl;
		t0 = clock();

		std::cout << "List Polynomial 1 ";
		P_list1.Print();
		std::cout << "List Polynomial 2 ";
		P_list2.Print();
		std::cout << "Map Polynomial 1 ";
		P_map1.Print();
		std::cout << "Map Polynomial 2 ";
		P_map2.Print();

//		std::cout << "P_list1+P_list2= ";
		P_list1 + P_list2;
//		(P_list1 + P_list2).Print();
		std::cout << "Time for List Polynomial Addition: " << clock() - t0 << "ms" << std::endl;
		t0 = clock();

//		std::cout << "P_list1-P_list2= ";
		P_list1 - P_list2;
//		(P_list1 - P_list2).Print();
		std::cout << "Time for List Polynomial subtraction: " << clock() - t0 << "ms" << std::endl;
		t0 = clock();

//		std::cout << "P_list1*P_list2= ";
		P_list1 * P_list2;
//		(P_list1 * P_list2).Print();
		std::cout << "Time for List Polynomial Multiplication: " << clock() - t0 << "ms" << std::endl;
		t0 = clock();

//		std::cout << "P_map1+P_map2= ";
		P_map1 + P_map2;
//		(P_map1 + P_map2).Print();
		std::cout << "Time for Map Polynomial Addition: " << clock() - t0 << "ms" << std::endl;
		t0 = clock();

//		std::cout << "P_map1-P_map2= ";
		P_map1 - P_map2;
//		(P_map1 - P_map2).Print();
		std::cout << "Time for Map Polynomial subtraction: " << clock() - t0 << "ms" << std::endl;
		t0 = clock();

//		std::cout << "P_map1*P_map2= ";
		P_map1 * P_map2;
//		(P_map1 * P_map2).Print();
		std::cout << "Time for Map Polynomial Multiplication: " << clock() - t0 << "ms" << std::endl;
		t0 = clock();

		return true;
	}
};

#endif // TESTPOLYNOMIAL_H
