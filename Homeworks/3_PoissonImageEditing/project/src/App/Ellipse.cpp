#include "Ellipse.h"
using namespace poissonedit;
Ellipse::Ellipse()
{
	type_ = kEllipse;
}

Ellipse::~Ellipse()
{
}

void Ellipse::Draw(QPainter& painter)
{
	painter.drawEllipse(start.x(), start.y(),
		end.x() - start.x(), end.y() - start.y());
}
