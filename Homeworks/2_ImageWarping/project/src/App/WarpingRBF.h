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
	Eigen::MatrixXd ImageWarping(QImage& image);

private:
	void Get_T();
	void Get_weight(QPoint p);

private:
	QVector <double> weight;
	double weight_sum;
	int mu;
};

#endif

