#ifndef WARPINGIDW_H
#define WARPINGIDW_H

#include "Warping.h"
#include <ANN/ANN.h>

class WarpingIDW :public Warping
{
public:
	WarpingIDW();
	~WarpingIDW();
	void InitAnchor(QVector<QPoint>src_list_, QVector<QPoint>tar_list_);
	QPoint PointConvert(QPoint p);
	void ImageWarping(QImage& image);

private:
	void Get_T();
	void Get_weight(QPoint p);

private:
	Eigen::MatrixXd T;
	QVector <double> weight;
	double weight_sum;
	int mu;
};

#endif

