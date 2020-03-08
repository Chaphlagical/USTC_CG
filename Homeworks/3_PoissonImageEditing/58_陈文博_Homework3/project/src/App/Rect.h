#pragma once

#include"Shape.h"
#include "Poisson.h"

namespace poissonedit
{
	class Rect :
		public Shape
	{
		public:
			Rect();
			~Rect();

			void Draw(QPainter& painter);
	};
}


