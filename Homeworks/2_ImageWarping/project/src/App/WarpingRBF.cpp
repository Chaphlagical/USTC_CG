#include "WarpingRBF.h"

WarpingRBF::WarpingRBF()
{
	weight_x.resize(0);
	weight_x.setZero();
	weight_y.resize(0);
	weight_y.setZero();
	win_width = 0;
	win_height = 0;
	mu = 0.5;
	weight_sum = 0;
}

WarpingRBF::WarpingRBF(int width, int height)
{
	mu = 0.5;
	weight_sum = 0;
	win_width = width;
	win_height = height;
}

WarpingRBF::~WarpingRBF()
{
}

void WarpingRBF::InitAnchor(QVector<QPoint>src_list_, QVector<QPoint>tar_list_)
{
	p_points = src_list_;
	q_points = tar_list_;

	for (int i = 0; i < p_points.size(); i++)
	{
		p_points[i] = QPoint(p_points[i].x() - win_width, p_points[i].y() - win_height);
		q_points[i] = QPoint(q_points[i].x() - win_width, q_points[i].y() - win_height);
	}
	if (p_points.size() <= 1)
	{
		return;
	}

	// get radius
	for (int i = 0; i < p_points.size(); i++)
	{
		int d_min = 1000000;
		for (int j = 0; j < p_points.size(); j++)
		{
			if (i != j)
			{
				int dist = Distance(p_points[i], p_points[j]);
				if (dist < d_min)
				{
					d_min = dist;
				}
			}
		}
		radius.push_back(d_min);
	}
	Get_weight();
}

void WarpingRBF::Get_weight()
{
	Eigen::MatrixXd A(p_points.size(), p_points.size());
	Eigen::VectorXd b_x(p_points.size());
	Eigen::VectorXd b_y(p_points.size());
	A.setZero(); b_x.setZero(); b_y.setZero();
	for (int i = 0; i < A.rows(); i++)
	{
		for (int j = 0; j < A.cols(); j++)
		{
			A(i, j) = pow(pow(Distance(p_points[i], p_points[j]), 2) + radius[j] * radius[j], mu);		
		}
		b_x(i) = q_points[i].x() - p_points[i].x();
		b_y(i) = q_points[i].y() - p_points[i].y();
	}
	weight_x.resize(p_points.size());
	weight_y.resize(p_points.size());
	weight_x = A.inverse() * b_x;
	weight_y = A.inverse() * b_y;
}

QPoint WarpingRBF::PointConvert(QPoint p)
{
	double convert_x = 0;
	double convert_y = 0;

	for (int i = 0; i < p_points.size(); i++)
	{
		convert_x += weight_x(i) * pow(pow(Distance(p_points[i], p), 2) + radius[i] * radius[i], mu);
		convert_y+=weight_y(i)* pow(pow(Distance(p_points[i], p), 2) + radius[i] * radius[i], mu);
	}
	return QPoint(convert_x+p.x(),convert_y+p.y());
}
