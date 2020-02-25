#ifndef WARPING_H
#define WARPING_H

#include <qvector.h>
#include <qimage.h>
#include <Eigen/Dense>
#include <ANN/ANN.h>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

class Warping
{
public:
	Warping();
	Warping(int width, int height);
	virtual ~Warping();
	virtual void InitAnchor(QVector<QPoint>src_list_, QVector<QPoint>tar_list_)=0;
	virtual QPoint PointConvert(QPoint p)=0;
	virtual Eigen::MatrixXd ImageWarping(QImage& image)=0;
	double Distance(QPoint p, QPoint q);
	// void FillHole(QImage& image, Eigen::MatrixXd& mask);
	void FillHole(QImage& image, Eigen::MatrixXd& mask, int threshold=25, int count=4);
	QRgb convolution_(QImage& image, Eigen::MatrixXd& kernel, int x, int y);	// convolution for one pixel
	void Convolution(QImage& image, Eigen::MatrixXd& kernel);	// Image convolution	
	QRgb Find_Nearby(QImage& image, int x, int y, int depth = 5);

public:
	int win_width;
	int win_height;

protected:
	QVector<QPoint> p_points;
	QVector<QPoint> q_points;
};


#endif 