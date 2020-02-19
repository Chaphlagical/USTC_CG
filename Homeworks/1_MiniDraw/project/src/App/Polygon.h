#ifndef POLYGON_H
#define POLYGON_H

#include"shape.h"

namespace minidraw {
	class Poly :public Shape
	{
	public:
		Poly();
		~Poly();

		void Draw(QPainter& painter);
	};

}



#endif 