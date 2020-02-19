#ifndef POLYGON_H
#define POLYGON_H

#include"shape.h"

class Polygon :public Shape
{
public:
	Polygon();
	~Polygon();

	void Draw(QPainter& painter);
};

#endif 