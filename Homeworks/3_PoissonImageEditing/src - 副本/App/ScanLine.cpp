#include "ScanLine.h"

ScanLine::ScanLine()
{

}

ScanLine::~ScanLine()
{

}

void ScanLine::InitPoints(QPoint start_point, QPoint end_point)
{
	start_x_ = start_point.x() < end_point.x() ? start_point.x() : end_point.x();
	start_y_ = start_point.y() < end_point.y() ? start_point.y() : end_point.y();
	width_ = abs(start_point.x() - end_point.x())+1;
	height_ = abs(start_point.y() - end_point.y())+1;
}

void ScanLine::InitPoints(QPainterPath path)
{

}

void ScanLine::InitPoints(QPolygon polygon)
{

}

void ScanLine::GetInsideMask()
{
	if (type_ == kRect)
	{
		inside_mask_.resize(height_, width_);
		inside_mask_.setOnes();
	}

	//return inside_mask_;
}