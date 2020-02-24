#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
	choose_status_ = false;
}


ImageWidget::~ImageWidget(void)
{
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	//QRect rect = QRect( (width()-ptr_image_->width())/2, (height()-ptr_image_->height())/2, ptr_image_->width(), ptr_image_->height());
	QRect rect = QRect(0, 0, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_); 

	// Draw points and lines
	painter.setPen(QPen(Qt::red, 4));
	for (int i = 0; i < src_list_.size(); i++)
	{
		painter.drawLine(src_list_[i], tar_list_[i]);
	}
	if (draw_status_)
	{
		painter.drawLine(start_, end_);
	}
	painter.end();
}

void ImageWidget::Open()
{
	choose_status_ = false;

	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;
	update();
}

void ImageWidget::Save()
{
	choose_status_ = false;

	SaveAs();
}

void ImageWidget::SaveAs()
{
	choose_status_ = false;

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	choose_status_ = false;

	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	choose_status_ = false;

	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
				}
			}
		}
		
	}
	else
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	choose_status_ = false;

	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}
	
	update();
}

void ImageWidget::Restore()
{
	choose_status_ = false;
	*(ptr_image_) = *(ptr_image_backup_);
	src_list_.clear();
	tar_list_.clear();
	update();
}

void ImageWidget::Choose()
{
	choose_status_ = true;
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (choose_status_&&Qt::LeftButton == mouseevent->button())
	{
		draw_status_ = true;
		start_ = end_ = mouseevent->pos();
		update();
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	if (choose_status_&&draw_status_)
	{
		end_ = mouseevent->pos();
		update();
	}
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	if (choose_status_&&draw_status_)
	{
		src_list_.push_back(start_);
		tar_list_.push_back(end_);
		draw_status_ = false;
		update();
	}
}

void ImageWidget::IDW()
{
	WarpingIDW warping;
	warping.InitAnchor(src_list_, tar_list_);
	warping.ImageWarping(*(ptr_image_));
	src_list_.clear();
	tar_list_.clear();
	update();
}