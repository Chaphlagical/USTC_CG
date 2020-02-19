#include "Line.h"

using namespace minidraw;

Line::Line()
{
	type_ = kLine;
}

Line::~Line()
{
}

void Line::Draw(QPainter& painter)
{
	painter.drawLine(start, end);
}
