#ifndef PARAMETERIZE_ASAP_H
#define PARAMETERIZE_ASAP_H
#include <Engine/MeshEdit/Paramaterize_Basic.h>
#include <Eigen/Sparse>
#include <ctime>


namespace Ubpa
{
	class TriMesh;

	class Parameterize_ASAP :
		public Paramaterize_Basic
	{
	public:
		Parameterize_ASAP(Ptr<TriMesh> triMesh);
		~Parameterize_ASAP();

	private:
		void Parameterization();

		void Init_Matrix();
		void Solve();



	private:
		size_t Get_a_index(size_t t);
		size_t Get_b_index(size_t t);
		veci2 Get_u_index(size_t index);
		std::vector<std::vector<double>> Get_delta_list(size_t index);
		//void Map_Add(coff_map map1, coff_map& result, double coef);
		coff_map Map_Add(coff_map map1, coff_map map2);
		double Map_Derivation(coff_map map_, size_t elem);
		coff_map Map_Multiply(coff_map map_, double coef);

		std::vector<coff_map> Get_elem_map(size_t t, size_t i);
		bool map_update(size_t index, coff_map& map);

	//private:


	private:
		typedef struct
		{
			size_t u;	//	uti index
			size_t u1;	//	uti+1 index
		}Aux_coef;


	private:
		std::vector<std::vector<Aux_coef>> aux_list;
		Eigen::VectorXd b_vector;
		Eigen::SimplicialLLT<Eigen::SparseMatrix<double>> solver;
	};
}
#endif