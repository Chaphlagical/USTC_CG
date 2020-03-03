#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ChildWindow.h"


using std::cout;
using std::endl;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	image_ = new QImage();
	image_backup_ = new QImage();

	draw_status_ = kNone;
	is_choosing_ = false;
	is_pasting_ = false;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	source_window_ = NULL;
	shape_ = NULL;
}

ImageWidget::~ImageWidget(void)
{
}

int ImageWidget::ImageWidth()
{
	//return image_->width();
	return image_mat_.cols;
}

int ImageWidget::ImageHeight()
{
	//return image_->height();
	return image_mat_.rows;
}

void ImageWidget::set_draw_status_to_choose()
{
	draw_status_ = kChoose;	
}

void ImageWidget::set_draw_status_to_paste()
{
	draw_status_ = kPaste;
}

const cv::Mat& ImageWidget::image()
{
	return image_mat_;
}

void ImageWidget::set_source_window(ChildWindow* childwindow)
{
	source_window_ = childwindow;
}

void ImageWidget::paintEvent(QPaintEvent* paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	//QRect rect = QRect(0, 0, image_->width(), image_->height());
//	QRect rect = QRect(0, 0, image_mat_.rows, image_mat_.s);
	*(image_) = QImage((unsigned char*)(image_mat_.data), image_mat_.cols, image_mat_.rows, image_mat_.step, QImage::Format_RGB888);
	QRect rect = QRect(0, 0, image_->width(), image_->height());
	painter.drawImage(rect, *image_);
	//cv::imshow("a",image_mat_);
	//cv::waitKey(0);

	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	if (shape_ != NULL)
	{
		shape_->Draw(painter);
	}
	//painter.drawRect(point_start_.x(), point_start_.y(),
	//	point_end_.x() - point_start_.x(), point_end_.y() - point_start_.y());

	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		switch (draw_status_)
		{
		case kChoose:
			is_choosing_ = true;
			point_start_ = point_end_ = mouseevent->pos();
			shape_->set_start(point_start_);
			shape_->set_end(point_end_);
			break;

		case kPaste:
		{
			is_pasting_ = true;

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image

			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;
			inside_mask = source_window_->imagewidget_->shape_->inside_mask_;
			
			//source_window_->imagewidget_->shape_->GetPoisson(QPoint(xpos,ypos),image_mat_);


			// Paste
			//if ((xpos + w < image_->width()) && (ypos + h < image_->height()))
			if ((xpos + w <image_mat_.cols) && (ypos + h < image_mat_.rows))
			{
				// Restore image
//				*(image_) = *(image_backup_);
				//image_mat_ = image_mat_backup_.clone();

				// Paste
				/*for (int i = 0; i < h; i++)
				{
					for (int j = 0; j < w; j++)
					{
						if (inside_mask(i, j) == 1)
						{
							image_mat_.at<cv::Vec3b>(ypos+i, xpos+j) = source_window_->imagewidget_->image_mat_.at<cv::Vec3b>(ysourcepos + i, xsourcepos + j );
							//image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
							//QRgb color;
							//color = qRgb(Poisson_mask.at<cv::Vec3b>(i, j)[0], Poisson_mask.at<cv::Vec3b>(i, j)[1], Poisson_mask.at<cv::Vec3b>(i, j)[2]);
							//image_->setPixel(xpos + i, ypos + j, color);
						}

					}
				}*/
				source_window_->imagewidget_->shape_->GetPoisson(mouseevent->pos(), source_window_->imagewidget_->point_start_,
					image_mat_, source_window_->imagewidget_->image_mat_);
			}
			
		}
		
		
		update();
		break;

		default:
			break;
		}
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		// Store point position for rectangle region
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			shape_->set_end(point_end_);
		}
		break;

	case kPaste:
		// Paste rectangle region to object image
		if (is_pasting_)
		{
			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;
			//Eigen::MatrixXi inside_mask = source_window_->imagewidget_->shape_->inside_mask_;

			// Paste
	//		if ((xpos > 0) && (ypos > 0) && (xpos + w < image_->width()) && (ypos + h < image_->height()))
			if ((xpos > 0) && (ypos > 0) && (xpos + w < image_mat_.cols) && (ypos + h < image_mat_.rows))
			{
				// Restore image 
				//*(image_) = *(image_backup_);
				image_mat_ = image_mat_backup_.clone();

				// Paste
				/*for (int i = 0; i < h; i++)
				{
					for (int j = 0; j < w; j++)
					{
						if (inside_mask(i, j) == 1)
						{
							image_mat_.at<cv::Vec3b>(ypos+i, xpos+j) = source_window_->imagewidget_->image_mat_.at<cv::Vec3b>(ysourcepos + i, xsourcepos + j);
							//image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
						}
					}
				}*/

				source_window_->imagewidget_->shape_->GetPoisson(mouseevent->pos(), source_window_->imagewidget_->point_start_, image_mat_,source_window_->imagewidget_->image_mat_);
			}
		}

	default:
		break;
	}

	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		if (is_choosing_)
		{
			
			point_end_ = mouseevent->pos();
			is_choosing_ = false;
			draw_status_ = kNone;
			if (shape_->type_ == Shape::kRect)
			{
				shape_->InitPoints(point_start_, point_end_);
				shape_->GetInsideMask();
				shape_->PoissonInit(image_mat_);
			}

		}

	case kPaste:
		if (is_pasting_)
		{
			is_pasting_ = false;
			draw_status_ = kNone;
		}

	default:
		break;
	}

	update();
}

void ImageWidget::Open(QString filename)
{
	// Load file
	if (!filename.isEmpty())
	{
		image_mat_ = cv::imread(filename.toStdString());
		cv::cvtColor(image_mat_, image_mat_, cv::COLOR_BGR2RGB);
		image_mat_backup_ = image_mat_.clone();
		std::cout << image_mat_.cols << " " << image_mat_.rows<<" " << image_mat_.step << std::endl;
		
		//image_->load(filename);
		//*(image_backup_) = *(image_);
	}

	//	setFixedSize(image_->width(), image_->height());
	//	relate_window_->setWindowFlags(Qt::Dialog);
	//	relate_window_->setFixedSize(QSize(image_->width(), image_->height()));
	//	relate_window_->setWindowFlags(Qt::SubWindow);

		//image_->invertPixels(QImage::InvertRgb);
		//*(image_) = image_->mirrored(true, true);
		//*(image_) = image_->rgbSwapped();
	//cout << "image size: " << image_->width() << ' ' << image_->height() << endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}

	//image_->save(filename);
	cv::imwrite(filename.toStdString(), image_mat_);
}

void ImageWidget::Invert()
{
	/*for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			image_->setPixel(i, j, qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color)));
		}
	}*/
	image_mat_ = 255 - image_mat_;

	// equivalent member function of class QImage
	// image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	/*QImage image_tmp(*(image_));
	int width = image_->width();
	int height = image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, height - 1 - j));
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(i, height - 1 - j));
				}
			}
		}

	}
	else
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, j));
				}
			}
		}
	}*/
	cv::flip(image_mat_, image_mat_,1);
	// equivalent member function of class QImage
	//*(image_) = image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	/*for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			int gray_value = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
			image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
		}
	}*/
	cv::cvtColor(image_mat_,image_mat_, cv::COLOR_RGB2GRAY);

	update();
}

void ImageWidget::Restore()
{
	image_mat_ = image_mat_backup_.clone();
	point_start_ = point_end_ = QPoint(0, 0);
	update();
}
