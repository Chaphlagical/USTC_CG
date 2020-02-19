#pragma once

#include"Shape.h"

namespace minidraw
{
	class Rect :public Shape
	{
		public:
			Rect();
			~Rect();

			void Draw(QPainter& painter);
	};
}


