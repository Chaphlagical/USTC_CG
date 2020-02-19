# include "Freedraw.h"

Freedraw::Freedraw()
{
}

Freedraw::~Freedraw()
{
}

void Freedraw::Draw(QPainter& painter)
{
	if (path.elementCount() == 0)
		path.moveTo(end);
	else
		path.lineTo(end);
	painter.drawPath(path);
}
