#include <iostream>

#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

int main() {
	/*MatrixXf A(4, 4);

	float data[4][4] = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};

	for (size_t row = 0; row < 4; row++) {
		for (size_t col = 0; col < 4; col++)
			A(row, col) = data[row][col];
	}

	MatrixXf b(4, 2);
	for (size_t row = 0; row < 4; row++) {
		b(row, 0) = 1;
		b(row, 1) = 2;
	}

	MatrixXf x = A.colPivHouseholderQr().solve(b);

	cout << x << endl;*/

	MatrixXd A(4, 4);
	Vector4d V;
	V << 1, 2, 3, 4;
	A << 1, 2, 3,0,
		4, 5, 6,0,
		7, 8, 9,0,
		10, 11, 12,0;
	//Map<MatrixXd> B(A.col(1).data(),2,2);
	//MatrixXd B = A.col(1).reshaped(2,2).transpose();
	//MatrixXd D = A.col(1).reshaped(2, 2).transpose();
	Eigen::MatrixXd kernel(3, 3);
	double v = 1 / 3;
	kernel << 0.3, 0.3, 0.3,
		0.3, 0.0, 0.3,
		0.3, 0.3, 0.3;
	std::cout << kernel << std::endl;
	return 0;
}
