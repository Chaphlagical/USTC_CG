#ifndef CURVE_H
#define CURVE_H

#include"shape.h"
#include <qvector.h>

namespace minidraw {
	class Curve :public Shape
	{
	public:
		Curve();
		~Curve();

		void Draw(QPainter& painter);
		void update(int mode);
	private:
		QPolygon polygon;
		QVector<QPoint> points;
		bool finish;
	};

}



#endif 