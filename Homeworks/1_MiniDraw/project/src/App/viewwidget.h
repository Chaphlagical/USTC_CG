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
	bool draw_status_;	//���Ƶ�ǰ״̬
	QPoint start_point_;	//����ͼԪ����ʼ��
	QPoint end_point_;	//��ǰͼԪ���յ�
	QColor line_color;
	QColor fill_color;
	int width;
	Shape::Type type_;
	Shape* shape_;
	std::vector<Shape*> shape_list_;
	int mode;


public:
	void mousePressEvent(QMouseEvent* event);	//��������Ӧ����
	void mouseMoveEvent(QMouseEvent* event);	//����ƶ���Ӧ����
	void mouseReleaseEvent(QMouseEvent* event);	//����ͷ���Ӧ����

public:
	void paintEvent(QPaintEvent*);	//Qt���еĻ��ƶ�ֻ���ڴ˺��������
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
