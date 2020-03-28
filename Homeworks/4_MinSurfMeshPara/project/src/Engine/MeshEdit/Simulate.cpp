#include <Engine/MeshEdit/Simulate.h>


#include <Eigen/Sparse>

using namespace Ubpa;

using namespace std;
using namespace Eigen;


void Simulate::Clear() {
	this->positions.clear();
	this->velocity.clear();
}

bool Simulate::Init() {
	//Clear();

	original_length_list.resize(edgelist.size()/2);
	mass = 1;

	//	Init extra force
	fext.resize(positions.size());

	this->velocity .resize(positions.size());
	for (int i = 0; i < positions.size(); i++)
	{
		fext[i] = vecf3(0, -gravity, 0);
		for (int j = 0; j < 3; j++)
		{
			this->velocity[i][j] = 0;
		}
	}

	for (size_t i = 0; i < edgelist.size() /2; i++)
	{
		pointf3 p1 = positions[edgelist[i*2]];
		pointf3 p2 = positions[edgelist[i*2+1]];
		original_length_list[i] = pointf3::distance(p1,p2);
	}
	//Init_Matrix_optimize();
	//cout << "original_length_list:" << endl << original_length_list[0] << endl << original_length_list[1] << endl << original_length_list[2] << endl<<endl;

	return true;
}

bool Simulate::Run() {
	//cout << "positions:" << endl << positions[0] << endl << positions[1] << endl << positions[2] << endl << endl;
	//cout << "velocity:" << endl << velocity[0] << endl << velocity[1] << endl << velocity[2] << endl << endl;
	SimulateOnce();
	
	//cout << "------------------------------------------------------------------------" << endl << endl;
	// half-edge structure -> triangle mesh

	return true;
}

void Ubpa::Simulate::SetLeftFix()
{
	//固定网格x坐标最小点
	fixed_id.clear();
	fixed_coords.clear();
	double x = 100000;
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i][0] < x)
		{
			x = positions[i][0];
		}
	}

	for (int i = 0; i < positions.size(); i++)
	{
		if (abs(positions[i][0] - x) < 1e-5)
		{
			fixed_id.push_back(i);
			fixed_coords.push_back(positions[i]);
		}
	}

	Init();
}

void Ubpa::Simulate::SetUpFix()
{
	//固定网格y坐标最大点
	fixed_id.clear();
	fixed_coords.clear();
	double y = -100000;
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i][1] > y)
		{
			y= positions[i][1];
		}
	}

	for (int i = 0; i < positions.size(); i++)
	{
		if (abs(positions[i][1] - y) < 1e-5)
		{
			fixed_id.push_back(i);
			fixed_coords.push_back(positions[i]);
		}
	}

	Init();
}

void Ubpa::Simulate::SetDownFix()
{
	//固定网格y坐标最小点
	fixed_id.clear();
	fixed_coords.clear();
	double y = 100000;
	for (int i = 0; i < positions.size(); i++)
	{
		if (positions[i][1] < y)
		{
			y = positions[i][1];
		}
	}

	for (int i = 0; i < positions.size(); i++)
	{
		if (abs(positions[i][1] - y) < 1e-2)
		{
			fixed_id.push_back(i);
			fixed_coords.push_back(positions[i]);
		}
	}

	Init();
}

void Simulate::SimulateOnce() {
	// TODO
	////cout << "WARNING::Simulate::SimulateOnce:" << endl;
//		<< "\t" << "not implemented" << endl;

	/*Init_Iteration();
	err = 10000;
	while (abs(err) > 1e-3)
	{
		IterationOnce();
		//cout << err << endl;
	}*/
	//cout << err << endl;
	//cout << "------------------------------------------------" << endl;
	
	Init_Matrix_optimize();
	err = 10000;
	while (abs(err) > 1e-3)
	{
		IterationOnce_optimize();
		//cout << err << endl;
	}

	Speed_Update();
	cout << "-----------------------------------------------------" << endl;

	for (size_t i = 0; i < fixed_id.size(); i++)
	{
		velocity[fixed_id[i]] = pointf3(0, 0, 0);
	}


}

void Simulate::Step_Update()
{
	vector<Eigen::Matrix3f> gradiant_force;
	vector<Eigen::Vector3f> g_list;
	std::vector<vecf3> fint_list;
	Eigen::Matrix3f unit_mat;

	gradiant_force.resize(positions.size());
	g_list.resize(positions.size());
	fint_list.resize(positions.size());



	unit_mat << 1, 0, 0, 
						0, 1, 0, 
						0, 0, 1;
	
	for (int i = 0; i < positions.size(); i++)
	{
		gradiant_force[i].setZero();
		g_list[i].setZero();
		fint_list[i] = vecf3(0, 0, 0);
	}


	for (size_t i = 0; i < edgelist.size()/2; i++)
	{
		size_t index_i = edgelist[i*2];
		size_t index_j = edgelist[i*2+1];

		pointf3 pi = positions[index_i];
		pointf3 pj = positions[index_j];
		
		vecf3 rij = pi - pj;
		
		float l0 = original_length_list[i];
		
		vecf3 temp_f= stiff * (rij.norm() - l0) / rij.norm() * (pj - pi);
		fint_list[index_i] += temp_f;
		fint_list[index_j] -= temp_f;

		Eigen::Vector3f r_vec;
		Eigen::Matrix3f r_mat;
		
		r_vec << rij[0], rij[1], rij[2];
		r_mat = r_vec * r_vec.transpose();

		Matrix3f temp_grad= stiff * (l0 / rij.norm() - 1) * unit_mat - stiff * l0 / pow(rij.norm(), 3) * r_mat;
		gradiant_force[index_i] += temp_grad;
		gradiant_force[index_j] -= temp_grad;
	}

	//cout << "fint:" << endl << fint_list[0] << endl << fint_list[1] << endl << fint_list[2] << endl << endl;

	//cout << "gradiant_force:" << endl << gradiant_force[0] << endl << gradiant_force[1] << endl << gradiant_force[2] << endl << endl;
	//for (size_t i = 0; i < positions.size(); i++)
	//{
		////cout << i << endl << gradiant_force[i] << endl << endl;
		////cout << edgelist[i/2] << endl;
	//}
	
	step_list.clear();
	for (int i = 0; i < positions.size(); i++)
	{
		pointf3 x = positions[i];
		Eigen::Matrix3f gradiant_g;
		Eigen::Vector3f g;
		Eigen::Vector3f step;
		vecf3 g_vec;

		g_vec = mass * (x - y_list[i]) - h * h * fint_list[i];
		g << g_vec[0], g_vec[1], g_vec[2];

		gradiant_g.setZero();

		gradiant_g = mass * unit_mat - h * h * gradiant_force[i];

		step = gradiant_g.inverse() * g;

		step_list.push_back(step);
		////cout << step << endl << endl;
	}
	//cout << "step_list:" << endl << step_list[0] << endl << step_list[1] << endl << step_list[2] << endl << endl;
	////cout << "----------------------------------------------------------------------" << endl;
}

void Simulate::Init_y_list()
{
	y_list.resize(positions.size());
	for (size_t i = 0; i < y_list.size(); i++)
	{
		vecf3 v(velocity[i][0], velocity[i][1], velocity[i][2]);
		y_list[i] = positions[i] + h * v + h * h * fext[i] / mass;
		velocity[i] = pointf3(positions[i][0], positions[i][1], positions[i][2]);
		////cout << y_list[i] << endl << endl;
	}
	for (size_t i = 0; i < fixed_id.size(); i++)
	{
		y_list[fixed_id[i]] = fixed_coords[i];
	}
	//cout << "y: " << y_list[0] << endl << y_list[1] << endl << y_list[2] << endl << endl;
}

void Simulate::Init_Iteration()
{
	Init_y_list();

	for (size_t i = 0; i < positions.size(); i++)
	{
		positions[i] = y_list[i];
	}
}

void Simulate::IterationOnce()
{
	Step_Update();
	err = 0;
	for (int i = 0; i < positions.size(); i++)
	{
		Eigen::Vector3f x_vec;
		x_vec << positions[i][0], positions[i][1], positions[i][2];
		x_vec -= step_list[i];
		err += abs(step_list[i][0]) + abs(step_list[i][1]) + abs(step_list[i][2]);
		positions[i] = pointf3(x_vec[0], x_vec[1], x_vec[2]);
		////cout << i<<endl<<step_list[i]<<endl<<positions[i]<<endl<<step_list[i]<<endl<<endl;
	}
	err /= positions.size();
	for (size_t i = 0; i < fixed_coords.size(); i++)
	{
		positions[fixed_id[i]] = fixed_coords[i];
	}
	////cout << "*********************************************************************";
}

void Simulate::Speed_Update()
{
	for (size_t i = 0; i < velocity.size(); i++)
	{
		vecf3 new_p(positions[i][0], positions[i][1], positions[i][2]);
		vecf3 old_p(velocity[i][0], velocity[i][1], velocity[i][2]);
		vecf3 v = (new_p - old_p) / h;
		velocity[i] = pointf3(v[0], v[1], v[2]);
	}
}

Eigen::MatrixXf Simulate::Kronecker_product(Eigen::MatrixXf A, Eigen::MatrixXf B)
{
	Eigen::MatrixXf result(A.rows() * B.rows(), A.cols() * B.cols());
	result.setZero();
	for (size_t A_row = 0; A_row < A.rows(); A_row++)
	{
		for (size_t A_col = 0; A_col < A.cols(); A_col++)
		{
			for (size_t B_row = 0; B_row < B.rows(); B_row++)
			{
				for (size_t B_col = 0; B_col < B.cols(); B_col++)
				{
					result(A_row * B.rows() + B_row, A_col * B.cols() + B_col) = A(A_row, A_col) * B(B_row, B_col);
				}
			}
		}
	}
	return result;
}

void Simulate::removeRow(Eigen::MatrixXf& matrix, unsigned int rowToRemove)
{
	unsigned int numRows = matrix.rows() - 1;
	unsigned int numCols = matrix.cols();

	if (rowToRemove < numRows)
		matrix.block(rowToRemove, 0, numRows - rowToRemove, numCols) = matrix.block(rowToRemove + 1, 0, numRows - rowToRemove, numCols);

	matrix.conservativeResize(numRows, numCols);
}
/////////////////////////////////////Optimize Method/////////////////////////////////////////////////////


void Simulate::Init_Matrix_optimize()
{
	vector<Eigen::VectorXf> A_list;
	vector<Eigen::VectorXf> S_list;
	Eigen::MatrixXf identity_mat(3, 3);
	identity_mat.setIdentity();

	size_t m = positions.size();
	size_t s = edgelist.size() / 2;
	
	S_list.resize(s);
	A_list.resize(s);
	L_mat.resize(m, m);
	J_mat.resize(m, s);

	L_mat.setZero();
	J_mat.setZero();

	for (size_t i = 0; i < s; i++)
	{
		A_list[i].resize(m);
		A_list[i].setZero();
		S_list[i].resize(s);
		S_list[i].setZero();

		A_list[i](edgelist[i * 2]) = 1;
		A_list[i](edgelist[i * 2 + 1]) = -1;
		S_list[i](i) = 1;
	}

	for(size_t i = 0; i < s; i++)
	{
		L_mat += A_list[i] * A_list[i].transpose() * stiff;
		J_mat += A_list[i] * S_list[i].transpose() * stiff;
	}

	Eigen::MatrixXf K_mat(m, m);
	K_mat.setIdentity();

	for (size_t i = 0; i < fixed_id.size(); i++)
	{
		removeRow(K_mat, fixed_id[i]);
	}

	identity_mat.resize(L_mat.rows(), L_mat.cols());

	A_optim_mat = mass * identity_mat + h * h * L_mat;
	//cout << A_optim_mat << endl<<endl;
	//cout << K_mat << endl << endl;
	//cout << (mass * identity_mat + h * h * L_mat) << endl << endl;

	solver.compute((K_mat * A_optim_mat * K_mat.transpose()).sparseView().pruned());
	Init_y_list();
	y_mat.resize(L_mat.rows(), 3);

	for (size_t i = 0; i < L_mat.rows(); i++)
	{
		y_mat(i, 0) = y_list[i][0];
		y_mat(i, 1) = y_list[i][1];
		y_mat(i, 2) = y_list[i][2];
	}

	

}

void Simulate::Update_d()
{
	d_vec.resize(edgelist.size() / 2, 3);
	d_vec.setZero();

	for (size_t i = 0; i < edgelist.size() / 2; i++)
	{
		pointf3 pi = positions[edgelist[i * 2]];
		pointf3 pj = positions[edgelist[i * 2 + 1]];
		vecf3 d;
		d = original_length_list[i] * (pi - pj) / pointf3::distance(pi, pj);
		d_vec(i, 0) = d[0];
		d_vec(i, 1) = d[1];
		d_vec(i, 2) = d[2];
	}
}

void Simulate::IterationOnce_optimize()
{
	Update_d();

	Eigen::MatrixXf B;
	
	Eigen::MatrixXf K_mat(positions.size(), positions.size());
	K_mat.setIdentity();
	
	for (size_t i = 0; i < fixed_id.size(); i++)
	{
		removeRow(K_mat, fixed_id[i]);
	}

	Eigen::MatrixXf X(positions.size(), 3);
	for (size_t i = 0; i < positions.size(); i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			X(i, j) = positions[i][j];
		}
	}

	Eigen::MatrixXf bb = X - K_mat.transpose() * K_mat * X;

	B = K_mat * (h * h * J_mat * d_vec + mass * y_mat - MatrixXf(A_optim_mat)*bb);

	X = solver.solve(B);
	cout << X << endl << endl << endl;
	err = 0;
	
	for (size_t i = 0; i < 3; i++)
	{
		cout << i << " " << positions[i] << endl;
	}
	cout << endl;
	for (size_t i = 0; i < K_mat.rows(); i++)
	{
		float err_temp = 0;
		cout << i  <<" "<<  X(i, 0) << " " << X(i, 1) << " " << X(i, 2) << endl;
		
		MatrixXf::Index index;
		K_mat.row(i).maxCoeff(&index);
		for (size_t j = 0; j < 3; j++)
		{
			err += abs(X(i, j) - positions[index][j]);
			err_temp+= abs(X(i, j) - positions[index][j]);
			
		}
		positions[index] = pointf3(X(i, 0), X(i, 1), X(i, 2));
		
	}
	cout << endl;
	for (size_t i = 0; i < 3; i++)
	{
		cout << i << " " << positions[i] << endl;
	}
	err /= positions.size();
	cout << endl;
	cout << "err: " << err << endl;
	cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl << endl;
	
	/*for (size_t i = 0; i < fixed_coords.size(); i++)
	{
		positions[fixed_id[i]] = fixed_coords[i];
	}*/
	//cout << err << endl;
	//cout << "J:" << endl << J_mat << endl << endl;
	//cout << "d:" << endl << d_vec << endl << endl;
	//cout << "y:" << endl << y_mat << endl << endl;
	//cout << "**************************************************" << endl;
}