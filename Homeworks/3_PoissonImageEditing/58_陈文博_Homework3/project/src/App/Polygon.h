#ifndef POLYGON_H
#define POLYGON_H

#include"Shape.h"

namespace poissonedit {
	class Polygon :public Shape
	{
		public:
			Polygon();
			~Polygon();

			void Draw(QPainter& painter);
			void update(int mode);
			QPolygon get_polygon();

		private:
			QPolygon polygon;
			bool finish;
	};

}



#endif 