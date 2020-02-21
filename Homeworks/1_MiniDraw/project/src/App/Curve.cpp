#include "Curve.h"

using namespace minidraw;

Curve::Curve()
{
	type_ = kCurve;
	finish = false;
	polygon.push_back(start);
}

Curve::~Curve()
{
}

void Curve::update(int mode)
{
	switch (mode)
	{
	case 0:
		finish = true;
		break;
	case 1:
		if (polygon.size() > 0)
		{
			polygon.back() = end;
			points.push_back(end);
		}
		polygon.push_back(polygon.back());
		break;
	default:
		break;
	}
}


void Curve::Draw(QPainter& painter)
{
	if (finish)
	{
		QPainterPath path(points[0]);
		for (int i = 0; i < points.size() - 1; i++)
		{
			QPoint sp = points[i];
			QPoint ep = points[i + 1];
			QPoint c1 = QPoint((sp.x() + ep.x()) / 2, sp.y());
			QPoint c2 = QPoint((sp.x() + ep.x()) / 2, ep.y());
			//QPoint c2 = QPoint(ep.x(), (sp.y() + ep.y()) / 2);
			path.cubicTo(c1, c2, ep);
			//path.quadTo(c1, ep);
		}
		painter.drawPath(path);
	}
		//painter.drawPolygon(polygon);
	else
		painter.drawPolyline(polygon);
}
