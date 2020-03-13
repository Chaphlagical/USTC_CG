#pragma once

#include <Basic/HeapObj.h>
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Eigen/Sparse>
#include <Engine/MeshEdit/Boundary_Circle.h>
#include <Engine/MeshEdit/Boundary_Basic.h>
#include <Engine/MeshEdit/Boundary_Square.h>

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	// mesh boundary == 1
	class Paramaterize : public HeapObj {
	public:
		Paramaterize(Ptr<TriMesh> triMesh);
	public:
		static const Ptr<Paramaterize> New(Ptr<TriMesh> triMesh) {
			return Ubpa::New<Paramaterize>(triMesh);
		}
	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		
		void Set_Display();

		void Set_Boundary_Square();
		void Set_Boundary_Circle();

		void Set_Uniform_Method();
		void Set_Cotangent_Method();
		
		bool Run();

	public:
		enum Boundary_Type
		{
			kCircle,
			kSquare
		}boundary_type;

		enum Barycentric_Type
		{
			kUniform,
			kCotangent
		}barycentric_type;

		enum Display_Status
		{
			koff,
			kon
		}display_status;

	private:
		void Paramaterization();
		void Init_Boundary();
		void Set_Boundary();
		void Init_Matrix_Uniform();
		void Init_Matrix_Cotangent();
		void Init_Matrix();

		//	Sparse Matrix Predecomposition
		void Predecomposition();

		void Solve();

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

	private:
		double Distance(V* v1, V* v2);
		double Cosine(V* v0, V* v1, V* v2);
		double Cotangent(V* v0, V* v1, V* v2);

	private:
		Ptr<TriMesh> triMesh;
		const Ptr<HEMesh<V>> heMesh; // vertice order is same with triMesh
		Ptr< Boundary_Basic> Boundary_Setting;
		Eigen::SparseMatrix<double> Laplace_matrix;
		Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
		std::vector<size_t> boundary_index;
		std::vector<pointf2> boundary_list;
		std::vector<pointf2>texcoords;
	};
}
