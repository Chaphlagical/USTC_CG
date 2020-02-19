#ifndef FREEDRAW_H
#define FREEDRAW_H

#include"shape.h"
#include <qpainterpath.h>

namespace minidraw
{
	class Freedraw :public Shape
	{
		public:
			Freedraw();
			~Freedraw();

			void Draw(QPainter& painter);

		protected:
			QPainterPath path;
	};
}


#endif