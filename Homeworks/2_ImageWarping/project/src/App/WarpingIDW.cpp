#include "WarpingIDW.h"
#include <iostream>

WarpingIDW::WarpingIDW()
{
	mu = -2;
	weight_sum = 0;
}

WarpingIDW::~WarpingIDW()
{

}

void WarpingIDW::InitAnchor(QVector<QPoint>src_list_, QVector<QPoint>tar_list_)
{
	p_points = src_list_;
	q_points = tar_list_;
	//std::cout << "bbbb";
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
//	for (int i =0;i< weight.size(); i++)
//		std::cout << weight[i]/weight_sum << " ";
//	std::cout << std::endl;
	//std::cout << weight[0] << std::endl;
	if (weight.empty())
	{
		return p;
	}
	else
	{
		for (int i = 0; i < weight.size(); i++)
		{
			//std::cout << "aaaa"<<std::endl;
			//std::cout << T;
			Eigen::MatrixXd T_ = T.row(i).reshaped(2, 2);
			//std::cout << "ccc";
			Eigen::Vector2d vector_q_i;
			Eigen::Vector2d vector_p_i;
			vector_q_i<< q_points[i].x(), q_points[i].y();
			vector_p_i << p_points[i].x(), p_points[i].y();
			convert_vactor += (weight[i]/weight_sum)*(vector_q_i + T_ * (vector_p - vector_p_i));
			//std::cout <<vector_p - vector_p_i << std::endl;
		}
	}
	return QPoint(convert_vactor.x(),convert_vactor.y());
}

void WarpingIDW::ImageWarping(QImage& image)
{
	//Eigen::MatrixXd new_image(image.width(), image.height);
	//new_image.setZero();
	QImage image_copy(image);
	QPoint convert_point;

	for (int i = 0; i < image.width(); i++)
	{
		for (int j = 0; j < image.height(); j++)
		{
			image.setPixel(i, j, qRgb(255, 255, 255));
		}
	}
	
	for (int i = 0; i < image.width(); i++)
	{
		for (int j = 0; j < image.height(); j++)
		{
			convert_point = PointConvert(QPoint(i, j));
			//std::cout << i<<" "<<j<<" "<<convert_point.x() << " " << convert_point.y() << std::endl;
			if (convert_point.x() > 0 && convert_point.x() < image.width())
			{
				if (convert_point.y() > 0 && convert_point.y() < image.height())
				{
					image.setPixel(convert_point, image_copy.pixel(i, j));
					//std::cout << convert_point.x() << " " << convert_point.y() << std::endl;
				}
			}
		}
	}
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
				T_ = (A.inverse().transpose())*B;
				T(i, 0) = T_(0, 0);
				T(i, 1) = T_(0, 1);
				T(i, 2) = T_(1, 0);
				T(i, 3) = T_(1, 1);
			}
		}
	}
	//std::cout << T;
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