#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include "ui_viewwidget.h"

#include "shape.h"
#include "Line.h"
#include "Rect.h"
#include "Ellipse.h"
#include "Freedraw.h"
#include "Polygon.h"

#include <qevent.h>
#include <qpainter.h>
#include <qcolordialog.h>
#include <qinputdialog.h>
#include <qfiledialog.h>
#include <QWidget>

#include <vector>

class ViewWidget : public QWidget
{
	Q_OBJECT

public:
	ViewWidget(QWidget* parent = 0);
	~ViewWidget();

private:
	Ui::ViewWidget ui;

private:
	bool draw_status_;	//绘制当前状态
	QPoint start_point_;	//生成图元的起始点
	QPoint end_point_;	//当前图元的终点
	QColor line_color;
	QColor fill_color;
	int width;
	Shape::Type type_;
	Shape* shape_;
	std::vector<Shape*> shape_list_;
	int mode;


public:
	void mousePressEvent(QMouseEvent* event);	//鼠标击发响应函数
	void mouseMoveEvent(QMouseEvent* event);	//鼠标移动响应函数
	void mouseReleaseEvent(QMouseEvent* event);	//鼠标释放响应函数

public:
	void paintEvent(QPaintEvent*);	//Qt所有的绘制都只能在此函数中完成
signals:
public slots:
	void setLine();
	void setRect();
	void setEllipse();
	void setFreedraw();
	void setPolygon();
	void undo();
	void setLineColor();
	void setFillColor();
	void setWidth();
	void save();
};

#endif // VIEWWIDGET_H
