#include "Warping.h"
#include"ANN/ANN.h"

Warping::Warping()
{
	win_height = 0;
	win_width = 0;
}

Warping::Warping(int width, int height)
{
	win_width = width;
	win_height = height;
}

Warping::~Warping()
{
}

double Warping::Distance(QPoint p, QPoint q)
{
	double d = ((double)p.x() - (double)q.x()) * ((double)p.x() - (double)q.x()) 
		+ ((double)p.y() - (double)q.y()) * ((double)p.y() - (double)q.y());
	return sqrt(d);
}


void Warping::Convolution(QImage& image, Eigen::MatrixXd& kernel)
{
	QImage img_copy(image);
	for (int i = 0; i < image.width(); i++)
	{
		for (int j = 0; j < image.height(); j++)
		{
			image.setPixel(i, j, convolution_(img_copy, kernel, i, j));
		}
	}
}

QRgb Warping::convolution_(QImage& image, Eigen::MatrixXd& kernel, int x, int y)
{
	int low_row = x - (int)floor(kernel.rows()/2);
	int up_row = x + (int)floor(kernel.rows()/2)+1;
	int low_col = y - (int)floor(kernel.cols()/2);
	int up_col = y + (int)floor(kernel.cols()/2)+1;
	low_row = (low_row < 0 ? 0 : low_row);
	up_row = (up_row > image.width() ? image.width() - 1 : up_row);
	low_col = (low_col < 0 ? 0 : low_col);
	up_col = (up_col > image.height() ? image.height() - 1 : up_col);

	QVector<double> colors(3);
	int count = 0;
	colors[0] = colors[1] = colors[2]=0;
	for (int i = low_row; i < up_row; i++)
	{
		for (int j = low_col; j < up_col; j++)
		{
			QRgb pixel = image.pixel(i, j);
			double weight =kernel(count++);
			colors[0] += qRed(pixel)*weight;
			colors[1] += qGreen(pixel)*weight;
			colors[2] += qBlue(pixel)*weight;
		}
	}
	return qRgb(colors[0], colors[1], colors[2]);
}


void Warping::FillHole(QImage& image, Eigen::MatrixXd& mask, int threshold, int count)
{
	int real_count = count;
	ANNpointArray dataPts = annAllocPts(image.height() * image.width(), 2);
	int nPts = 0;

	for (int i = 0; i < image.width(); i++)
	{
		for (int j = 0; j < image.height(); j++)
		{
			if (mask(i, j) == 1)
			{
				dataPts[nPts][0] = i;
				dataPts[nPts][1] = j;
				nPts++;
			}
		}
	}

	ANNkd_tree* kdtree = new ANNkd_tree(dataPts, nPts, 2);

	for (int i = 0; i < image.width(); i++)
	{
		for (int j = 0; j < image.height(); j++)
		{
			
			
				QVector<int> colors(3);
				colors[0] = colors[1] = colors[2] = 0;
				ANNpoint pt = annAllocPt(2);
				ANNidxArray index = new ANNidx[count];
				ANNdistArray dist = new ANNdist[count];
				pt[0] = i;
				pt[1] = j;
				kdtree->annkSearch(pt, count, index, dist, 0);

				real_count = count;
				for (int m = 0; m < count; m++)
				{
					int x = dataPts[index[m]][0];
					int y = dataPts[index[m]][1];
					QRgb rgb = image.pixel(x, y);
					if (fabs(x - i) <= threshold && fabs(y - j) <= threshold)
					{
						colors[0] += qRed(rgb);
						colors[1] += qGreen(rgb);
						colors[2] += qBlue(rgb);
					}
					else
					{
						real_count--;
					}
				}
				if (real_count > 0)
				{
					image.setPixel(i, j, qRgb(colors[0] / real_count, colors[1] / real_count, colors[2] / real_count));
				}
				delete[] index;
				delete[] dist;
		}
	}
	delete kdtree;
}