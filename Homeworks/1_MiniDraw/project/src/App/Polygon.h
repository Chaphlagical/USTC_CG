#ifndef POLYGON_H
#define POLYGON_H

#include"shape.h"

namespace minidraw {
	class Polygon :public Shape
	{
	public:
		Polygon();
		~Polygon();

		void Draw(QPainter& painter);
	};

}



#endif 