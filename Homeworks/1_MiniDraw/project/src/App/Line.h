#pragma once

#include"shape.h"

namespace minidraw
{
	class Line :public Shape
	{
	public:
		Line();
		~Line();

		void Draw(QPainter& painter);
	};
}
