#ifndef ELLIPSE_H
#define ELLIPSE_H

#include"shape.h"

namespace minidraw
{
	class Ellipse :public Shape
{
public:
	Ellipse();
	~Ellipse();

	void Draw(QPainter& painter);
};
}


#endif 