#include <Engine/MeshEdit/Boundary_CIrcle.h>
using namespace Ubpa;
Boundary_Circle::Boundary_Circle()
{
}

Boundary_Circle::~Boundary_Circle()
{
}

void Boundary_Circle::Set_Boundary(std::vector<size_t> boundary_index, std::vector<pointf2>& boundary_list)
{
	size_t nB = boundary_index.size();
	double theta = 2 * PI<double> / (double)nB;
	for (int i = 0; i < nB; i++)
	{
		boundary_list.push_back(pointf2(0.5 * cos(i * theta) + 0.5, 0.5 * sin(i * theta) + 0.5));
	}
}
