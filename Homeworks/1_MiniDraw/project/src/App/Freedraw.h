#ifndef FREEDRAW_H
#define FREEDRAW_H

#include"shape.h"
#include <qpainterpath.h>

class Freedraw :public Shape
{
public:
	Freedraw();
	~Freedraw();

	void Draw(QPainter& painter);

protected:
	QPainterPath path;
};

#endif