#include "ScanLine.h"

ScanLine::ScanLine()
{
	m_pEDGEs = NULL;
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
	x_min_ = start_x_ = start_point.x() < end_point.x() ? start_point.x() : end_point.x();
	y_min_ = start_y_ = start_point.y() < end_point.y() ? start_point.y() : end_point.y();
	x_max_ = end_x_ = start_point.x() > end_point.x() ? start_point.x() : end_point.x();
	y_max_ = end_y_ = start_point.y() > end_point.y() ? start_point.y() : end_point.y();
	width_ = abs(start_point.x() - end_point.x())+1;
	height_ = abs(start_point.y() - end_point.y())+1;
	points_.push_back(QPoint(start_point.x(),start_point.y()));
	points_.push_back(QPoint(start_point.x(), end_point.y()));
	points_.push_back(QPoint(end_point.x(), end_point.y()));
	points_.push_back(QPoint(end_point.x(), start_point.y()));
	inside_mask_.resize(height_, width_);
	inside_mask_.setZero();
	m_PtsNum = points_.size();
}
#include <iostream>
void ScanLine::InitPoints(QPainterPath path)
{
	points_ = path.toFillPolygon().toPolygon();
	x_min_ = y_min_ = 100000;
	x_max_ = y_max_ = -1;
	for (int i = 0; i < points_.size(); i++)
	{
		if (points_[i].x() < x_min_)
			x_min_ = points_[i].x();
		if (points_[i].x() > x_max_)
			x_max_ = points_[i].x();
		if (points_[i].y() < y_min_)
			y_min_ = points_[i].y();
		if (points_[i].y() > y_max_)
			y_max_ = points_[i].y();
	}
	width_ = x_max_ - x_min_ + 1;
	height_ = y_max_ - y_min_ + 1;
	start_x_ = x_min_;
	start_y_ = y_min_;
	end_x_ = x_max_;
	end_y_ = y_max_;
	inside_mask_.resize(height_, width_);
	inside_mask_.setZero();
	m_PtsNum = points_.size();
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
	points_.pop_back();
	m_PtsNum = points_.size();

}

void ScanLine::GetInsideMask()
{
	inside_mask_.setZero();
	InitEDGEs();
	InitNET();
	InitAET();
	FillPolygon();
	/*cv::Point* pointlist = new cv::Point[points_.size()];
	for (int i = 0; i < points_.size(); i++)
	{
		pointlist[i] = cv::Point(points_[i].x() - x_min_, points_[i].y() - y_min_);
	}
	const cv::Point* ppt[1] = { pointlist };
	int npt[] = { points_.size() };
	cv::Mat img_mask(height_ + 1, width_ + 1, CV_8UC1, cv::Scalar(255));
	cv::fillPoly(img_mask, ppt, npt, 1, cv::Scalar(1));

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
	if (pointlist != NULL)
	{
		delete[] pointlist;
	}*/
}

// Init EDGEs
void ScanLine::InitEDGEs()
{
	if (m_pEDGEs)
	{
		delete[] m_pEDGEs; m_pEDGEs = NULL;
	}
	m_pEDGEs = new EDGE[m_PtsNum];

	for (int i = 0; i < m_PtsNum - 1; i++)
	{
		if (points_[i].y() > points_[i + 1].y())
		{
			m_pEDGEs[i].up = points_[i];
			m_pEDGEs[i].down = points_[i + 1];
		}
		else
		{
			m_pEDGEs[i].up = points_[i+1];
			m_pEDGEs[i].down = points_[i];
		}
		m_pEDGEs[i].eg.y_max = m_pEDGEs[i].up.y();
		m_pEDGEs[i].eg.x = m_pEDGEs[i].down.x();
		if (m_pEDGEs[i].up.y() == m_pEDGEs[i].down.y())
			m_pEDGEs[i].eg.dx = DBL_MAX;
		else
			m_pEDGEs[i].eg.dx = ((double)m_pEDGEs[i].up.x()- (double)m_pEDGEs[i].down.x())/((double)m_pEDGEs[i].up.y()- (double)m_pEDGEs[i].down.y());
	}

	if (points_[0].y() > points_[m_PtsNum - 1].y())
	{
		m_pEDGEs[m_PtsNum - 1].up = points_[0];
		m_pEDGEs[m_PtsNum - 1].down = points_[m_PtsNum - 1];
	}
	else
	{
		m_pEDGEs[m_PtsNum - 1].up = points_[m_PtsNum - 1]; 
		m_pEDGEs[m_PtsNum - 1].down = points_[0];
	}

	m_pEDGEs[m_PtsNum - 1].eg.y_max = m_pEDGEs[m_PtsNum - 1].up.y();
	m_pEDGEs[m_PtsNum - 1].eg.x = m_pEDGEs[m_PtsNum - 1].down.x();
	if (m_pEDGEs[m_PtsNum - 1].up.y() == m_pEDGEs[m_PtsNum - 1].down.y())
		m_pEDGEs[m_PtsNum - 1].eg.dx = DBL_MAX;
	else
		m_pEDGEs[m_PtsNum - 1].eg.dx =((double)m_pEDGEs[m_PtsNum - 1].up.x() - (double)m_pEDGEs[m_PtsNum - 1].down.x()) / ((double)m_pEDGEs[m_PtsNum - 1].up.y() - (double)m_pEDGEs[m_PtsNum - 1].down.y());
}

//	Init new edge table
void ScanLine::InitNET()
{
	NET_.clear();

	for (int i = 0; i < y_max_-y_min_+1; i++)
	{
		NET_.append(QVector<Edge>(0));
	}
	//	add new edge to NET
	for (int i = 0; i < m_PtsNum; i++)
	{
		int scanline = m_pEDGEs[i].down.y() - y_min_;
		NET_[scanline].push_back(m_pEDGEs[i].eg);
	}
	// sort edge according to x_min
	for (int n = y_min_; n < y_max_; n++)
	{
		int index = n - y_min_;
		int sz = NET_[index].size();
		for (int i = 0; i < sz - 1; i++)
		{
			for (int k = i + 1; k < sz; k++)
			{
				if (NET_[index][i].x > NET_[index][k].x)
				{
					Edge e = NET_[index][i];
					NET_[index][i] = NET_[index][k];
					NET_[index][k] = e;
				}
			}
		}
	}
}

// Init Active Edge Table
void ScanLine::InitAET()
{
	AET_.clear();
	for (int i = 0; i < y_max_ - y_min_ + 1; i++)
	{
		AET_.append(QVector<Edge>(0));
	}
}

void ScanLine::MoveNewEdgeFromNET()
{
	int index = scan_index - y_min_;
	for (int i = 0; i < NET_[index].size(); i++)
	{
		AET_[index].push_back(NET_[index][i]);
	}
}

//	delete old edge
void ScanLine::RemoveEdges()
{
	int index = scan_index - y_min_;
	for (int i = 0; i < AET_[index].size(); i++)
	{
		if (scan_index == AET_[index][i].y_max)
		{
			AET_[index].remove(i);
			i--;
		}
	}
}

// Sorting AET
void ScanLine::SortAET()
{
	int index = scan_index - y_min_;
	int sz = AET_[index].size();
	for (int i = 0; i < sz - 1; i++)
	{
		for (int k = i + 1; k < sz; k++)
		{
			if (AET_[index][i].x > AET_[index][k].x)
			{
				Edge e = AET_[index][i];
				AET_[index][i] = AET_[index][k];
				AET_[index][k] = e;
			}
		}
	}
}

// Fill the scanline
void ScanLine::FillScanLine()
{
	int index = scan_index - y_min_;
	for (int i = 0; i < AET_[index].size() - 1; i += 2)
	{
		for (int x0 = AET_[index][i].x ; x0 < (int)(AET_[index][i + 1].x); x0++)
		{
			inside_mask_(index, x0- x_min_) = 1;
		}
	}
}

// update dx in NET
void ScanLine::UpdateDeltaX()
{
	int index = scan_index - y_min_;
	int sz = AET_[index].size();
	for (int i = 0; i < sz; i++)
	{
		AET_[index][i].x += AET_[index][i].dx;
		AET_[index + 1].push_back(AET_[index][i]);
	}
}

// Fill Polygon
void ScanLine::FillPolygon()
{
	cv::Mat img_mask(height_ + 1, width_ + 1, CV_8UC1, cv::Scalar(255));
	for (scan_index = y_min_; scan_index < y_max_; scan_index++)
	{
		MoveNewEdgeFromNET();
		RemoveEdges();
		SortAET();
		FillScanLine();
		UpdateDeltaX();
	}
}