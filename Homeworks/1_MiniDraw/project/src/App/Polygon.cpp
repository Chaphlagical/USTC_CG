#include "Polygon.h"

Polygon::Polygon()
{
}

Polygon::~Polygon()
{
}

void Polygon::Draw(QPainter& painter)
{
	painter.drawEllipse(start.x(), start.y(),
		end.x() - start.x(), end.y() - start.y());
}
