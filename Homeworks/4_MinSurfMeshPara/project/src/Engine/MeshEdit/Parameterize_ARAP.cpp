#include <Engine/MeshEdit/Parameterize_ARAP.h>


using namespace Ubpa;
using namespace std;

Parameterize_ARAP::Parameterize_ARAP(Ptr<TriMesh> triMesh)
	:Paramaterize_Basic::Paramaterize_Basic(triMesh)
{
	
}

Parameterize_ARAP::~Parameterize_ARAP()
{
	Clear();
}

void Parameterize_ARAP::Parameterization()
{
	Param_Init();
	Local_Phase();
	Matrix_Init();
	for (size_t t = 0; t < iteration_times; t++)
	{
		B_Init();
		Solve();
		cout << t + 1 << " iteration complete" << endl;
	}
}

void Parameterize_ARAP::Local_Phase()
{
	Eigen::Matrix2d Lt, Jt;

	for (size_t t = 0; t < nP; t++)
	{
		Jt.setZero();
		for (int i = 0; i < 3; i++)
		{
			Eigen::Vector2d u_vec, x_vec;
			x_vec << delta_x_list[t][i], delta_y_list[t][i];
			u_vec << delta_u_list[t][i][0], delta_u_list[t][i][1];
			Jt += cotangent_list[t][i] * u_vec * x_vec.transpose();
		}
		Eigen::JacobiSVD<Eigen::MatrixXd> svd(Jt, Eigen::ComputeThinU | Eigen::ComputeThinV);
		Lt = svd.matrixU() * svd.matrixV().transpose();
		L_matrix_list.push_back(Lt);
	}
}

