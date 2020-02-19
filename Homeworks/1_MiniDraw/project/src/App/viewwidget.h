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
	Shape::Type type_;
	Shape* shape_;
	std::vector<Shape*> shape_list_;


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
};

#endif // VIEWWIDGET_H
