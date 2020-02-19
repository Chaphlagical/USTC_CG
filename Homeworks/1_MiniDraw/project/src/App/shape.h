#pragma once

#include <QtGui>

class Shape
{
public:
	Shape();
	virtual ~Shape();
	void set_start(QPoint s); 
	void set_end(QPoint e);
	virtual void Draw(QPainter& paint) = 0;
	virtual void update(int mode){}

public:
	QColor line_color;
	QColor fill_color;
	int width;

public:
	enum Type
	{
		kDefault = 0,
		kLine = 1,
		kRect = 2,
		kEllipse = 3,
		kPolygon = 4,
		kFreedraw = 5,
	};

protected:
	QPoint start;
	QPoint end;
};