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

protected:
	QPoint start;
	QPoint end;
};