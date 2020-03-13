#pragma once
#include <Engine/MeshEdit/Boundary_Basic.h>
#include <Eigen/Sparse>
#include <vector>

namespace Ubpa {
	class TriMesh;

	class Paramaterize_Uniform
	{
	public:
		Paramaterize_Uniform() {};
		virtual ~Paramaterize_Uniform() {};

	private:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };

	public:
		virtual void Init_Matrix(const Ptr<HEMesh<V>>& heMesh, Eigen::SparseMatrix<double>& Laplace_matrix);
	};
}