#include "Ellipse.h"

Ellipse::Ellipse()
{
}

Ellipse::~Ellipse()
{
}

void Ellipse::Draw(QPainter& painter)
{
	painter.drawLine(start, end);
}
