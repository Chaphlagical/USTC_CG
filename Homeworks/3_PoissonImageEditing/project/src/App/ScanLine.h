#pragma once
# include <QtGui>
#include <Eigen/Dense>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

typedef struct Edge
{
	int x;
	int dx;
	int y_max;
}Edge;

class ScanLine
{
public:
	ScanLine();
	~ScanLine();

public:
	void InitPoints(QPoint start_point, QPoint end_point);
	void InitPoints(QPainterPath path);
	void InitPoints(QPolygon polygon);
	void GetInsideMask();	//	Get inside mask
	Eigen::MatrixXi inside_mask_;
	QPoint get_start();
	QPoint get_end();

private:
	QVector <QPoint> points_;
	
	QVector<QList<Edge>> Net_;	//	New edge table
	QList<Edge> AET_;	//	active edge table
	int x_min_, x_max_, y_min_, y_max_;
	int width_, height_;
	int start_x_, start_y_, end_x_, end_y_ ;


};