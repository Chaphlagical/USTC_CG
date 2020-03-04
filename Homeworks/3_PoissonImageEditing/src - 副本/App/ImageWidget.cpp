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

	inside_mask.resize(0, 0);
}

ImageWidget::~ImageWidget(void)
{
	delete source_window_;
	delete shape_;
	image_mat_.release();
	image_mat_backup_.release();
	image_mat_last_.release();
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
			
			// Paste
			//if ((xpos + w < image_->width()) && (ypos + h < image_->height()))
			if ((xpos + w <image_mat_.cols) && (ypos + h < image_mat_.rows))
			{
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
				image_mat_ = image_mat_last_.clone();

				// Paste
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
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	cv::flip(image_mat_, image_mat_,1);
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
	update();
}

void ImageWidget::Restore()
{
	image_mat_ = image_mat_backup_.clone();
	point_start_ = point_end_ = QPoint(0, 0);
	update();
}
