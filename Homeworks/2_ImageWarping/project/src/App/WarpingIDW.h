#ifndef WARPINGIDW_H
#define WARPINGIDW_H

#include "Warping.h"

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
	void Get_weight();

private:
	Eigen::Matrix2d T;
	QVector <double> weight;

};

#endif 



