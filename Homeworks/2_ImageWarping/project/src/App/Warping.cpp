#include "Warping.h"

Warping::Warping()
{
}

Warping::~Warping()
{
}

double Warping::Distance(QPoint p, QPoint q)
{
	double d = ((double)p.x() - (double)q.x()) * ((double)p.x() - (double)q.x()) 
		+ ((double)p.y() - (double)q.y()) * ((double)p.y() - (double)q.y());
	return sqrt(d);
}

