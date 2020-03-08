# include "Freedraw.h"
using namespace poissonedit;
Freedraw::Freedraw()
{
	type_ = kFreedraw;
}

Freedraw::~Freedraw()
{
}

QPainterPath Freedraw::get_path()
{
	return path;
}

void Freedraw::Draw(QPainter& painter)
{
	if (path.elementCount() == 0)
		path.moveTo(end);
	else
		path.lineTo(end);
	painter.drawPath(path);
}
