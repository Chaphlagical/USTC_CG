#ifndef ELLIPSE_H
#define ELLIPSE_H

#include"Shape.h"

namespace poissonedit
{
	class Ellipse :public Shape
	{
		public:
			Ellipse();
			~Ellipse();

			void Draw(QPainter& painter);
	};

}

#endif 