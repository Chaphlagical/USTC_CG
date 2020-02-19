#include "Polygon.h"
using namespace minidraw;

Poly::Poly()
{
}

Poly::~Poly()
{
}

void Poly::Draw(QPainter& painter)
{
	painter.drawEllipse(start.x(), start.y(),
		end.x() - start.x(), end.y() - start.y());
}
