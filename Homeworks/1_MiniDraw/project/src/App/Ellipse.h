#ifndef ELLIPSE_H
#define ELLIPSE_H

#include"shape.h"

class Ellip :public Shape
{
public:
	Ellip();
	~Ellip();

	void Draw(QPainter& painter);
};

#endif 