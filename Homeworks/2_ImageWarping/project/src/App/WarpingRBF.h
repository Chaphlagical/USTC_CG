#ifndef WARPINGRBF_H
#define WARPINGRBF_H

#include "Warping.h"

class WarpingRBF :public Warping
{
public:
	WarpingRBF();
	WarpingRBF(int width, int height);
	~WarpingRBF();
	void InitAnchor(QVector<QPoint>src_list_, QVector<QPoint>tar_list_);
	QPoint PointConvert(QPoint p);
	//Eigen::MatrixXd ImageWarping(QImage& image);

private:
	void Get_weight();

private:
	Eigen::VectorXd weight_x, weight_y;
	QVector <double> radius;
	double weight_sum;
	double mu;
};

#endif

