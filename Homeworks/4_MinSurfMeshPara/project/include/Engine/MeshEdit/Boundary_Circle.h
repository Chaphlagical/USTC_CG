#pragma once
#include<Engine/MeshEdit/Boundary_Basic.h>
namespace Ubpa {
	class Boundary_Circle :
		public Boundary_Basic
	{
	public:
		Boundary_Circle();
		~Boundary_Circle();
	public:
		void Set_Boundary(std::vector<size_t> boundary_index, std::vector<pointf2>& boundary_list);
	};
}