#ifndef WARPING_H
#define WARPING_H

#include <qvector.h>
#include <qimage.h>
#include <Eigen/Dense>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

class Warping
{
public:
	Warping();
	virtual ~Warping();
	virtual void InitAnchor(QVector<QPoint>src_list_, QVector<QPoint>tar_list_)=0;
	virtual QPoint PointConvert(QPoint p)=0;
	virtual void ImageWarping(QImage& image)=0;
	double Distance(QPoint p, QPoint q);
	void FillHole(QImage& image, Eigen::MatrixXd& mask);

protected:
	QVector<QPoint> p_points;
	QVector<QPoint> q_points;
};


#endif 