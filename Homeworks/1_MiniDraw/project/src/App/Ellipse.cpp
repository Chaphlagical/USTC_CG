#include "Ellipse.h"

Ellip::Ellip()
{
}

Ellip::~Ellip()
{
}

void Ellip::Draw(QPainter& painter)
{
	painter.drawLine(start, end);
}
