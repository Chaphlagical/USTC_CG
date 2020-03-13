#include <Engine/MeshEdit/Boundary_Square.h>
using namespace Ubpa;
Boundary_Square::Boundary_Square()
{
}

Boundary_Square::~Boundary_Square()
{
}

void Boundary_Square::Set_Boundary(std::vector<size_t> boundary_index, std::vector<pointf2>& boundary_list)
{
	size_t nB = boundary_index.size();
	double step = 4 / (double)nB;
	for (size_t i = 0; i < nB; i++)
	{
		double temp = (double)i * step;
		if (temp < 1)
		{
			boundary_list.push_back(pointf2(0, temp));
		}
		else if (temp < 2 && temp >= 1)
		{
			boundary_list.push_back(pointf2(temp - 1, 1));
		}
		else if (temp < 3 && temp >= 2)
		{
			boundary_list.push_back(pointf2(1, 3 - temp));
		}
		else
		{
			boundary_list.push_back(pointf2(4 - temp, 0));
		}
	}
}
