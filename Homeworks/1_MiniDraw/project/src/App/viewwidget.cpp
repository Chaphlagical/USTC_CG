#include "viewwidget.h"

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	draw_status_ = false;	//设置初始绘制状态
	shape_ = NULL;
	type_ = Shape::kDefault;
	line_color = Qt::black;
	fill_color = QColor(190, 230, 250);
	fill_color.setAlphaF(0);
	width = 1;
	int mode = 1;
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

void ViewWidget::undo()
{
	if (shape_list_.size() > 0)
	{
		delete shape_list_.back();	//	不加delete将造成内存泄漏
		shape_list_.pop_back();
	}
}

void ViewWidget::setLineColor()
{
	line_color = QColorDialog::getColor(Qt::black, this);
}

void ViewWidget::setFillColor()
{
	QColor default_color = QColor(190, 230, 250);
	default_color.setAlphaF(0);
	fill_color = QColorDialog::getColor(default_color, this);
}

void ViewWidget::save()
{
	QPixmap pix = this->grab();	//获取当前截图
	QString strFile = "a.png";
	QString fileName = QFileDialog::getSaveFileName(this, "Save", strFile, "PNG (*.png);;BMP (*.bmp);;JPEG (*.jpg *.jpeg)");
	if (!fileName.isNull())
	{
		pix.save(fileName);
	}
}

void ViewWidget::setWidth()
{
	QInputDialog diag(this);
	diag.setWindowTitle("Input");
	diag.setLabelText("Input your line width:");
	if (diag.exec() == QInputDialog::Accepted)
	{
		width = diag.textValue().toInt();
	}
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
			if (shape_ == NULL)
			{
				shape_ = new minidraw::Polygon();
				setMouseTracking(true);
			}
			break;
		}
		if (shape_ != NULL)
		{
			shape_->line_color = line_color;
			shape_->fill_color = fill_color;
			shape_->width = width;
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
		if (type_ != Shape::kPolygon)
		{
			draw_status_ = false;
			shape_list_.push_back(shape_);
			shape_ = NULL;
		}
		else {
			if (Qt::LeftButton == event->button())
			{
				shape_->update(1);
			}
			else if (Qt::RightButton == event->button())
			{
				shape_->update(0);
				shape_list_.push_back(shape_);
				shape_ = NULL;
			}
		}
	}
}
# include <iostream>
void ViewWidget::paintEvent(QPaintEvent*)
{
	QPainter painter(this);
	QColor default_color = QColor(190, 230, 250);
	QPen pen;
	QBrush brush;
	default_color.setAlphaF(0);

	for (int i = 0; i < shape_list_.size(); i++)
	{
		pen.setColor(shape_list_[i]->line_color);
		pen.setWidth(shape_list_[i]->width);
		if (shape_list_[i]->type_ != Shape::kFreedraw) {	//	自由绘画不设置填充色
			painter.setBrush(shape_list_[i]->fill_color);
		}
		else
			painter.setBrush(default_color);
		//brush.setColor(shape_list_[i]->fill_color);
		//painter.setBrush(brush);
		painter.setPen(pen);
		shape_list_[i]->Draw(painter);
	}

	if (shape_ != NULL) {
		shape_->Draw(painter);
	}

	update();	//更新窗口
}