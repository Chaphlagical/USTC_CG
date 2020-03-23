#include <Engine/MeshEdit/Parameterize_ASAP_GL.h>


using namespace Ubpa;
using namespace std;

Parameterize_ASAP_GL::Parameterize_ASAP_GL(Ptr<TriMesh> triMesh)
	:Paramaterize_Basic::Paramaterize_Basic(triMesh)
{
	
}

Parameterize_ASAP_GL::~Parameterize_ASAP_GL()
{
	Clear();
}

void Parameterize_ASAP_GL::Parameterization()
{
	vector<size_t> vec_num;
	vec_num.push_back(nV - 1);
	Set_Fixed_num(vec_num);
	Param_Init();
	Local_Phase();
	Matrix_Init();
	for (size_t t = 0; t < iteration_times; t++)
	{
		B_Init();
		Solve();
	}
}

void Parameterize_ASAP_GL::Local_Phase()
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
		Eigen::MatrixXd U = svd.matrixU();
		Eigen::MatrixXd V = svd.matrixV();
		double s = (svd.singularValues()(0) + svd.singularValues()(1)) / 2;
		Eigen::MatrixXd S(2,2);
		S << s, 0, 0, s;
		Lt = U * S * V.transpose();
		L_matrix_list.push_back(Lt);
	}
}



