#include "Line.h"
using namespace minidraw;
Line::Line()
{
}

Line::~Line()
{
}

void Line::Draw(QPainter& painter)
{
	painter.drawLine(start, end);
}
