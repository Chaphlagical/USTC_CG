#pragma once

#include <QtGui>
#include "Poisson.h"

class Shape:
	public Poisson
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
		kDefault,
		kRect,
		kEllipse,
		kPolygon,
		kFreedraw,
	};
	Type type_;

protected:
	QPoint start;
	QPoint end;
};