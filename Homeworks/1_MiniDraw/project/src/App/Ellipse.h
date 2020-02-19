#pragma once

#include"shape.h"

class Ellipse :public Shape
{
public:
	Ellipse();
	~Ellipse();

	void Draw(QPainter& painter);
};
