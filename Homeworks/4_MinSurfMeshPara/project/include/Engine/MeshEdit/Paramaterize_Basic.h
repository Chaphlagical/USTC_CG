#pragma once
#include <UHEMesh/HEMesh.h>
#include <UGM/UGM>
#include <Basic/HeapObj.h>
#include<Eigen/Sparse>
#include<Eigen/Dense>
#include <Engine/MeshEdit//Paramaterize.h>

namespace Ubpa {
	class TriMesh;
	class MinSurf;

	typedef std::map<size_t, int, std::greater<size_t>> index_map;
	typedef std::map<size_t, double, std::greater<size_t>> coff_map;
	typedef std::map<size_t, pointf2, std::greater<size_t>> pt_map;

	// mesh boundary == 1
	class Paramaterize_Basic
	{
	public:
		Paramaterize_Basic(Ptr<TriMesh> triMesh);
		void Set_Display();
		void Set_Iteration(float t);
		virtual ~Paramaterize_Basic();

	public:
		void Clear();
		bool Init(Ptr<TriMesh> triMesh);
		bool Run();

	protected:
		void Local_Flatten();
		void Param_Init();
		void Matrix_Init();
		void B_Init();
		void Solve();
		size_t Get_opposite_index(size_t i, size_t j);
		void Set_Fixed_num(std::vector<size_t> t);


	protected:
		virtual void  Parameterization()=0;
		virtual void Local_Phase() {};
		
	public:
		enum Para_Method
		{
			kASAP,
			kARAP
		};

	protected:
		class V;
		class E;
		class P;
		class V : public TVertex<V, E, P> {
		public:
			vecf3 pos;
		};
		class E : public TEdge<V, E, P> { };
		class P :public TPolygon<V, E, P> { };

	protected:
		Ptr<TriMesh> triMesh;
		Ptr<HEMesh<V>> heMesh;
		std::vector<pointf2>texcoords;

		size_t nP, nV;
		std::vector<std::vector<pointf2>> vertices_list;
		std::vector<index_map> vertices_index_polygon;
		std::vector<std::vector<size_t>> vertices_index_hemesh;
		std::vector<std::vector<double>> cotangent_list;
		std::vector<std::vector<double>> delta_x_list;
		std::vector<std::vector<double>> delta_y_list;
		Eigen::SparseMatrix<double> Coef_matrix;
		pt_map fixed_pt;
		bool display_status;
		size_t iteration_times;
		std::vector<size_t> fixed_num;

	protected:
		std::vector<pointf2> u_list;
		std::vector<std::vector<pointf2>> delta_u_list;
		std::vector<Eigen::Matrix2d> L_matrix_list;
		Eigen::MatrixXd B;
		Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

	public:
		Ptr<Paramaterize>paramaterize;
	};
}