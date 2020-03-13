#pragma once
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Basic/HeapObj.h>
#include<Eigen/Sparse>

namespace Ubpa {
	class TriMesh;
	class Paramaterize;

	class Paramaterize_Basic
	{
	public:
		Paramaterize_Basic() {};
		virtual ~Paramaterize_Basic() {};

	public:
		virtual void Init_Matrix(std::vector &heMesh, Eigen::SparseMatrix<double> &Laplace_matrix) {};
	};
}