#include "WarpingIDW.h"

WarpingIDW::WarpingIDW()
{
	win_width = 0;
	win_height = 0;
	mu = -2;
	weight_sum = 0;
}

WarpingIDW::WarpingIDW(int width, int height)
{
	mu = -2;
	weight_sum = 0;
	win_width = width;
	win_height = height;
}

WarpingIDW::~WarpingIDW()
{

}

void WarpingIDW::InitAnchor(QVector<QPoint>src_list_, QVector<QPoint>tar_list_)
{
	p_points = src_list_;
	q_points = tar_list_;
	for (int i = 0; i < p_points.size(); i++)
	{
		p_points[i] = QPoint(p_points[i].x() - win_width, p_points[i].y() - win_height);
		q_points[i] = QPoint(q_points[i].x() - win_width, q_points[i].y() - win_height);
	}
	T.resize(p_points.size(), 4);
	Get_T();
	
}

QPoint WarpingIDW::PointConvert(QPoint p)
{
	Eigen::Vector2d convert_vactor;
	Eigen::Vector2d vector_p;
	convert_vactor.setZero();
	vector_p << p.x(), p.y();
	Get_weight(p);
	if (weight.empty())
	{
		return p;
	}
	else
	{
		for (int i = 0; i < weight.size(); i++)
		{
			Eigen::MatrixXd T_ = T.row(i).reshaped(2, 2);
			Eigen::Vector2d vector_q_i;
			Eigen::Vector2d vector_p_i;
			vector_q_i<< q_points[i].x(), q_points[i].y();
			vector_p_i << p_points[i].x(), p_points[i].y();
			convert_vactor += (weight[i]/weight_sum)*(vector_q_i + T_ * (vector_p - vector_p_i));
		}
	}
	return QPoint(convert_vactor.x(),convert_vactor.y());
}

void WarpingIDW::Get_T()
{
	if (p_points.size() == 1)
	{
		T << 1, 0,
			0, 1;
	}
	else if (p_points.size() == 2)	// Singular matrix
	{
		for (int i = 0; i < 2; i++)
		{
			T(i, 0) = (q_points[1].x() - q_points[0].x()) / (p_points[1].x() - p_points[0].x());
			T(i, 1) = 0;
			T(i, 2) = 0;
			T(i, 3) = (q_points[1].y() - q_points[0].y()) / (p_points[1].y() - p_points[0].y());
		}
	}
	else
	{
		for (int i = 0; i < p_points.size(); i++)
		{
			Eigen::MatrixXd A(2, 2), B(2, 2), T_(2, 2);

			A.setZero();
			B.setZero();
			for (int j = 0; j < p_points.size(); j++)
			{
				if (i != j)
				{
					double sigma = pow(Distance(p_points[i], p_points[j]), mu);
					Eigen::Matrix2d diff_p;
					Eigen::Matrix2d diff_q;
					diff_p << (double)p_points[j].x() - (double)p_points[i].x(), 0, (double)p_points[j].y() - (double)p_points[i].y(), 0;
					diff_q << (double)q_points[j].x() - (double)q_points[i].x(), (double)q_points[j].y() - (double)q_points[i].y(), 0, 0;
					A += sigma * diff_p * diff_p.transpose();
					B += sigma * diff_p * diff_q;

				}
				T_ = (A.inverse())*B;
				T(i, 0) = T_(0, 0);
				T(i, 1) = T_(0, 1);
				T(i, 2) = T_(1, 0);
				T(i, 3) = T_(1, 1);
			}
		}
	}
}

void WarpingIDW::Get_weight(QPoint p)
{
	weight_sum = 0;
	weight.clear();
	for (int i = 0; i < p_points.size(); i++)
	{
		weight.push_back(pow(Distance(p_points[i], p),mu));
		weight_sum += weight[i];
	}
}