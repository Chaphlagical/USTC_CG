#pragma once

#include <Basic/HeapObj.h>
//#include <Engine/Primitive/MassSpring.h>
#include <UGM/UGM>
#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace Ubpa {
	class Simulate : public HeapObj {
	public:
		Simulate(const std::vector<pointf3>& plist,
			const std::vector<unsigned>& elist) {
			edgelist = elist;
			this->positions.resize(plist.size());
			for (int i = 0; i < plist.size(); i++)
			{
				for (int j = 0; j < 3; j++)
				{
					this->positions[i][j] = plist[i][j];
				}
			}
		};
	public:
		static const Ptr<Simulate> New(const std::vector<pointf3>& plist,
			const std::vector<unsigned> &elist) {
			return Ubpa::New<Simulate>(plist, elist);
		}
	public:
		// clear cache data
		void Clear();

		// init cache data (eg. half-edge structure) for Run()
		bool Init();
		//bool Init();

		// call it after Init()
		bool Run();
		
		const std::vector<pointf3>& GetPositions() const { return positions; };

		const float GetStiff() { return stiff; };
		void SetStiff(float k) { stiff = k; Init();};
		const float GetTimeStep() { return h; };
		void SetTimeStep(float k) { h = k; Init();};
		std::vector<unsigned>& GetFix() { return this->fixed_id; };
		void SetFix(const std::vector<unsigned>& f) { this->fixed_id = f; Init();};
		const std::vector<pointf3>& GetVelocity() { return velocity; };
		//void SetVelocity(const std::vector<pointf3>& v) { velocity = v; };

		void SetLeftFix();
		void SetUpFix();
		void SetDownFix();
		void AddFix();
		void ClearFix();
		
		const vecf3& GetFext() const { return fext[select_index]; }
		const size_t& GetIndex()const { return select_index; }
		const size_t& GetMaxIndex()const { return fext.size()-1; }
		void SetFext(const pointf3& f) {
			std::cout << select_index << std::endl;
			fext[select_index] = vecf3(f[0], f[1] - gravity, f[2]);
		}
		void SetFext_all(const pointf3& f) {
			for (size_t index = 0; index < fext.size(); index++)
				fext[index] = vecf3(f[0], f[1], f[2]);
		}

		void SetIndex(const size_t& index)
		{
			if (index < fext.size() - 1)
				select_index = index;
			else
				select_index = fext.size() - 1;
		}

	private:
		// kernel part of the algorithm
		void SimulateOnce();
		void Step_Update();
		void Init_y_list();
		void Init_Iteration();
		void IterationOnce();
		void Speed_Update();

	private:
		Eigen::MatrixXf Kronecker_product(Eigen::MatrixXf A, Eigen::MatrixXf B);
		void removeRow(Eigen::MatrixXf& matrix, unsigned int rowToRemove);
		
	//	optimize method
	private:
		void Init_Matrix_optimize();
		void Update_d();
		void Init_b_optim();
		void IterationOnce_optimize();

	//	optimize method
	private:
		Eigen::MatrixXf L_mat;
		Eigen::MatrixXf J_mat;
		Eigen::MatrixXf d_vec;

		Eigen::MatrixXf A_optim_mat;
		Eigen::MatrixXf y_mat;
		Eigen::MatrixXf b_optim_mat;

		Eigen::SimplicialLDLT<Eigen::SparseMatrix<float>> solver;

	private:
		float h = 0.03f;  //²½³¤
		float stiff =10000.00f;
		float mass;
		float err;
		std::vector<unsigned> fixed_id;  //fixed point id

		std::vector<pointf3> fixed_coords;	//fixed point coordinate


		//	force
		float gravity=9.8;
		std::vector<vecf3> fext;
		size_t select_index = 0;



		// gradient matrix
		std::vector<Eigen::Vector3f> step_list;

		// auxiliary 
		std::vector<pointf3> y_list;
		

		//mesh data
		std::vector<unsigned> edgelist;
		std::vector<float> original_length_list;


		//simulation data
		std::vector<pointf3> positions;
		std::vector<pointf3> velocity;

		
	};
}
