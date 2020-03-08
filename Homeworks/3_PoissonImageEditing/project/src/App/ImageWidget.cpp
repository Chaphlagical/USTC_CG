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
	draw_status_ = kNone;
	is_choosing_ = false;
	is_pasting_ = false;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	source_window_ = NULL;
	shape_ = NULL;
	scanline_ = NULL;
	poisson_ = NULL; 

	paste_status_ = 0;
	inside_mask_.resize(0, 0);
}

ImageWidget::~ImageWidget(void)
{
	image_mat_.release();
	image_mat_backup_.release();
	image_mat_last_.release();
	if (shape_ != NULL)
	{
		delete shape_;
	}
	if (scanline_ != NULL)
	{
		delete scanline_;
	}
	if (poisson_ != NULL)
	{
		delete poisson_;
	}
	
}

int ImageWidget::ImageWidth()
{
	return image_mat_.cols;
}

int ImageWidget::ImageHeight()
{
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

void ImageWidget::set_mix_paste()
{
	paste_status_ = MIX;
}

void ImageWidget::set_normal_paste()
{
	paste_status_ = NORMAL;
}

void ImageWidget::set_poisson_paste()
{
	paste_status_ = POISSON;
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
	QImage image_((unsigned char*)(image_mat_.data), image_mat_.cols, image_mat_.rows, image_mat_.step, QImage::Format_RGB888);
	QRect rect = QRect(0, 0, image_.width(), image_.height());
	painter.drawImage(rect, image_);

	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	if (shape_ != NULL)
	{
		shape_->Draw(painter);
	}

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
			if (shape_->type_ == Shape::kPolygon)
			{
				shape_->update(1);
			}
			break;

		case kPaste:
		{
			is_pasting_ = true;

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->scanline_->get_start().rx();
			int ysourcepos = source_window_->imagewidget_->scanline_->get_start().ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->scanline_->get_end().rx();
				- source_window_->imagewidget_->scanline_->get_start().rx() + 1;
			int h = source_window_->imagewidget_->scanline_->get_end().ry()
				- source_window_->imagewidget_->scanline_->get_start().ry() + 1;
			inside_mask_ = source_window_->imagewidget_->inside_mask_;
			
			// Paste
			if ((xpos + w <image_mat_.cols) && (ypos + h < image_mat_.rows))
			{
				switch (paste_status_)
				{
				case MIX:
					source_window_->imagewidget_->poisson_->MixingPoisson(mouseevent->pos(), source_window_->imagewidget_->scanline_->get_start(),
						image_mat_, source_window_->imagewidget_->image_mat_);
					break;
				case POISSON:
					source_window_->imagewidget_->poisson_->GetPoisson(mouseevent->pos(), source_window_->imagewidget_->scanline_->get_start(),
						image_mat_, source_window_->imagewidget_->image_mat_);
					break;
				case NORMAL:
					source_window_->imagewidget_->poisson_->CopyPaste(mouseevent->pos(), source_window_->imagewidget_->scanline_->get_start(),
						image_mat_, source_window_->imagewidget_->image_mat_);
					break;
				default:
					break;
				}
			}
		}
		update();
		break;
		default:
			break;
		}
	}
	if (Qt::RightButton == mouseevent->button())
	{
		if (draw_status_ == kChoose&&shape_->type_==Shape::kPolygon)
		{
			is_choosing_ = false;
			draw_status_ = kNone;
			scanline_ = new ScanLine;
			poisson_ = new Poisson;
			shape_->update(0);
			scanline_->InitPoints(shape_->get_polygon());
			scanline_->GetInsideMask();
			poisson_->set_insidemask(scanline_->inside_mask_);
			poisson_->PoissonInit(image_mat_);
			
			update();
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
			int xsourcepos = source_window_->imagewidget_->scanline_->get_start().rx();
			int ysourcepos = source_window_->imagewidget_->scanline_->get_start().ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->scanline_->get_end().rx()
				- source_window_->imagewidget_->scanline_->get_start().rx() + 1;
			int h = source_window_->imagewidget_->scanline_->get_end().ry()
				- source_window_->imagewidget_->scanline_->get_start().ry() + 1;

			// Paste
			if ((xpos > 0) && (ypos > 0) && (xpos + w < image_mat_.cols) && (ypos + h < image_mat_.rows))
			{
				// Restore image 
				image_mat_ = image_mat_last_.clone();
				// Paste
				switch (paste_status_)
				{
				case MIX:
					source_window_->imagewidget_->poisson_->MixingPoisson(mouseevent->pos(), source_window_->imagewidget_->scanline_->get_start(),
						image_mat_, source_window_->imagewidget_->image_mat_);
					break;
				case POISSON:
					source_window_->imagewidget_->poisson_->GetPoisson(mouseevent->pos(), source_window_->imagewidget_->scanline_->get_start(),
						image_mat_, source_window_->imagewidget_->image_mat_);
					break;
				case NORMAL:
					source_window_->imagewidget_->poisson_->CopyPaste(mouseevent->pos(), source_window_->imagewidget_->scanline_->get_start(),
						image_mat_, source_window_->imagewidget_->image_mat_);
					break;
				default:
					break;
				}
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
			if (shape_->type_ == Shape::kRect)
			{
				scanline_ = new ScanLine;
				poisson_ = new Poisson;
				is_choosing_ = false;
				draw_status_ = kNone;
				scanline_->InitPoints(point_start_, point_end_);
				scanline_->GetInsideMask();
				poisson_->set_insidemask(scanline_->inside_mask_);
				poisson_->PoissonInit(image_mat_);
			}
			if (shape_->type_ == Shape::kFreedraw)
			{
				shape_->set_end(point_start_);
				scanline_ = new ScanLine;
				poisson_ = new Poisson;
				is_choosing_ = false;
				draw_status_ = kNone;
				scanline_->InitPoints(shape_->get_path());
				scanline_->GetInsideMask();
				poisson_->set_insidemask(scanline_->inside_mask_);
				poisson_->PoissonInit(image_mat_);
			}
		}

	case kPaste:
		if (is_pasting_)
		{
			is_pasting_ = false;
			draw_status_ = kNone;
			image_mat_last_ = image_mat_.clone();
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
		image_mat_last_ = image_mat_.clone();
		std::cout << image_mat_.cols << " " << image_mat_.rows<<" " << image_mat_.step << std::endl;
	}
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
	cv::Mat img_save = image_mat_.clone();
	cv::cvtColor(img_save, img_save, cv::COLOR_RGB2BGR);
	cv::imwrite(filename.toStdString(), img_save);
}

void ImageWidget::Invert()
{
	for (int i = 0; i < image_mat_.rows; i++)
	{
		for (int j = 0; j < image_mat_.cols; j++)
		{
			image_mat_.at<cv::Vec3b>(i, j)[0] = 255 - image_mat_.at<cv::Vec3b>(i, j)[0];
			image_mat_.at<cv::Vec3b>(i, j)[1] = 255 - image_mat_.at<cv::Vec3b>(i, j)[1];
			image_mat_.at<cv::Vec3b>(i, j)[2] = 255 - image_mat_.at<cv::Vec3b>(i, j)[2];
		}
	}
	image_mat_last_ = image_mat_.clone();
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	cv::flip(image_mat_, image_mat_,1);
	image_mat_last_ = image_mat_.clone();
	update();
}

void ImageWidget::TurnGray()
{
	for (int i = 0; i < image_mat_.rows; i++)
	{
		for (int j = 0; j < image_mat_.cols; j++)
		{
			unsigned char gray = (image_mat_.at<cv::Vec3b>(i, j)[0] + image_mat_.at<cv::Vec3b>(i, j)[1] + image_mat_.at<cv::Vec3b>(i, j)[2])/3;
			image_mat_.at<cv::Vec3b>(i, j) = cv::Vec3b(gray, gray, gray);
		}
	}
	image_mat_last_ = image_mat_.clone();
	update();
}

void ImageWidget::Restore()
{
	if (shape_ != NULL)
	{
		delete shape_;
		shape_ = NULL;
	}

	image_mat_ = image_mat_backup_.clone();
	image_mat_last_ = image_mat_backup_.clone();
	point_start_ = point_end_ = QPoint(0, 0);
	update();
}
