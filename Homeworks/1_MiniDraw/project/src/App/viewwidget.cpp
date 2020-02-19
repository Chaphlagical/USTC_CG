#include "viewwidget.h"

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	draw_status_ = false;	//设置初始绘制状态
	shape_ = NULL;
	type_ = Shape::kDefault;
}

ViewWidget::~ViewWidget()
{
	for (int i = 0; i < shape_list_.size(); i++)
	{
		if (shape_list_[i])
		{
			delete shape_list_[i];
			shape_list_[i] = NULL;
		}
	}
}

void ViewWidget::setLine()
{
	type_ = Shape::kLine;
}

void ViewWidget::setRect()
{
	type_ = Shape::kRect;
}

void ViewWidget::setEllipse()
{
	type_ = Shape::kEllipse;
}

void ViewWidget::setFreedraw()
{
	type_ = Shape::kFreedraw;
}

void ViewWidget::setPolygon()
{
	type_ = Shape::kPolygon;
}

void ViewWidget::mousePressEvent(QMouseEvent* event)
{
	if (Qt::LeftButton == event->button())	//	判断是否鼠标左击
	{
		switch (type_)
		{
		case Shape::kLine:
			shape_ = new minidraw::Line();
			break;
		case Shape::kDefault:
			break;
		
		case Shape::kRect:
			shape_ = new minidraw::Rect();
			break;
		
		case Shape::kEllipse:
			shape_ = new minidraw::Ellipse();
			break;

		case Shape::kFreedraw:
			shape_ = new minidraw::Freedraw();
			break;

		case Shape::kPolygon:
			shape_ = new minidraw::Polygon();
			break;
		}
		if (shape_ != NULL)
		{
			draw_status_ = true;		//设置绘制状态为绘制
			start_point_ = end_point_ = event->pos();	//将图元初始点设置为当前鼠标出发点
			shape_->set_start(start_point_);
			shape_->set_end(end_point_);
		}
	}
	update();
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (draw_status_ && shape_ != NULL)	//判断当前绘制状态
	{
		end_point_ = event->pos();	//设置图元终止点位于鼠标当前位置
		shape_->set_end(end_point_);
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (shape_ != NULL)
	{
		draw_status_ = false;	//	设置绘制状态为绘制
		shape_list_.push_back(shape_);
		shape_ = NULL;
	}
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);	//	定义painter在this指向的控件中绘制

	for (int i = 0; i < shape_list_.size(); i++)
	{
		shape_list_[i]->Draw(painter);
	}

	if (shape_ != NULL) {
		shape_->Draw(painter);
	}

	update();	//更新窗口
}