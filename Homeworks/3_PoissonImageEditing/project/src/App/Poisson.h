#pragma once
#include "ScanLine.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <Eigen/Sparse>

class Poisson:
	public ScanLine
{
public:
	Poisson();
	~Poisson();
	void PoissonInit(cv::Mat source_img);
	
	void GetPoisson(QPoint paste_point, QPoint source_point, cv::Mat& paste_img_, cv::Mat& source_img_);

private:
	void Predecomposition();

private:
	//cv::Mat source_img_;
	QPoint source_point_, paste_point_;
	int pixels_num_;
	Eigen::MatrixXi index_matrix_;
	int width_, height_;
	Eigen::SparseMatrix<float> sparse_matrix_;
	Eigen::SimplicialLLT<Eigen::SparseMatrix<float>> solver;
	Eigen::VectorXf div_red_;
	Eigen::VectorXf div_green_ ;
	Eigen::VectorXf div_blue_ ;
};