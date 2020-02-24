#pragma

#include "Figure.h"

class Ellipse : public Figure {
public:
	Ellipse(const QPoint _start_point, const QPoint _end_point);
	~Ellipse();

	void Draw(QPainter& paint);
	// ���ڶ�̬����
	void update(const QPoint _end_point);
	void setPen(const QPen* pen_, const int mode);
	std::vector<QPoint*> getPoint(QPoint& pos, bool all);
private:
	QPoint start_point, end_point;
	QPen* boundPen;
};
