#include "ScanLine.h"

ScanLine::ScanLine()
{

}

ScanLine::~ScanLine()
{

}

QPoint ScanLine::get_start()
{
	return QPoint(start_x_, start_y_);
}

QPoint ScanLine::get_end()
{
	return QPoint(end_x_, end_y_);
}

void ScanLine::InitPoints(QPoint start_point, QPoint end_point)
{
	x_min_=start_x_ = start_point.x() < end_point.x() ? start_point.x() : end_point.x();
	y_min_=start_y_ = start_point.y() < end_point.y() ? start_point.y() : end_point.y();
	x_max_ = start_point.x() < end_point.x() ? start_point.x() : end_point.x();
	y_max_ = start_point.y() < end_point.y() ? start_point.y() : end_point.y();
	width_ = abs(start_point.x() - end_point.x())+1;
	height_ = abs(start_point.y() - end_point.y())+1;
	inside_mask_.resize(height_, width_);
	points_.push_back(QPoint(start_point.x(),start_point.y()));
	points_.push_back(QPoint(start_point.x(), end_point.y()));
	points_.push_back(QPoint(end_point.x(), end_point.y()));
	points_.push_back(QPoint(end_point.x(), start_point.y()));
}

void ScanLine::InitPoints(QPainterPath path)
{

}

void ScanLine::InitPoints(QPolygon polygon)
{
	x_min_ = y_min_ = 100000;
	x_max_ = y_max_ = -1;
	for (int i = 0; i < polygon.size(); i++)
	{
		if (polygon[i].x() < x_min_)
			x_min_ = polygon[i].x();
		if (polygon[i].x() > x_max_)
			x_max_ = polygon[i].x();
		if (polygon[i].y() < y_min_)
			y_min_ = polygon[i].y();
		if (polygon[i].y() > y_max_)
			y_max_ = polygon[i].y();
	}
	width_ = x_max_-x_min_+1;
	height_ = y_max_-y_min_ + 1;
	start_x_ = x_min_;
	start_y_ = y_min_;
	end_x_ = x_max_;
	end_y_ = y_max_;
	inside_mask_.resize(height_, width_);
	inside_mask_.setZero();
	points_ = polygon;
}

void ScanLine::GetInsideMask()
{
	inside_mask_.setZero();
	cv::Point* pointlist = new cv::Point[points_.size()];
	for (int i = 0; i < points_.size(); i++)
	{
		pointlist[i] = cv::Point(points_[i].x()-x_min_, points_[i].y() - y_min_);
	}
	const cv::Point* ppt[1] = { pointlist };
	int npt[] = {points_.size()};
	cv::Mat img_mask(height_+1, width_+1, CV_8UC1, cv::Scalar(255));
	cv::fillPoly(img_mask, ppt, npt,1, cv::Scalar(1));
	
	for (int i = 0; i < height_; i++)
	{
		for (int j = 0; j < width_; j++)
		{
			if (img_mask.at<unsigned char>(i, j) == 1)
			{
				inside_mask_(i, j) = 1;
			}
		}
	}
	delete[] pointlist;
	delete[] ppt;
	delete[] npt;

}

