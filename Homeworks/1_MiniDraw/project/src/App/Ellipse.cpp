#include "Ellipse.h"

Ellip::Ellip()
{
}

Ellip::~Ellip()
{
}

void Ellip::Draw(QPainter& painter)
{
	painter.drawEllipse(start.x(), start.y(),
		end.x() - start.x(), end.y() - start.y());
}
