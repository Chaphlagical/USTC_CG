# include "Freedraw.h"
using namespace minidraw;
Freedraw::Freedraw()
{
	type_ = kFreedraw;
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
