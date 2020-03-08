#pragma once
#include "ScanLine.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <Eigen/Sparse>
#include<Eigen/IterativeLinearSolvers>

class Poisson
{
public:
	Poisson();
	~Poisson();
	void PoissonInit(cv::Mat source_img);
	void set_insidemask(Eigen::MatrixXi inside_mask);
	void GetPoisson(QPoint paste_point, QPoint source_point, cv::Mat& paste_img_, cv::Mat& source_img_);
	void MixingPoisson(QPoint paste_point, QPoint source_point, cv::Mat& paste_img_, cv::Mat& source_img_);
	void CopyPaste(QPoint paste_point, QPoint source_point, cv::Mat& paste_img_, cv::Mat& source_img_);

private:
	void Predecomposition();
	double VecLength(cv::Vec3i vec);

private:
	//cv::Mat source_img_;
	QPoint source_point_, paste_point_;
	int pixels_num_;
	Eigen::MatrixXi index_matrix_;
	int width_, height_;
	Eigen::SparseMatrix<float> sparse_matrix_;
	Eigen::SimplicialLLT<Eigen::SparseMatrix<float>> solver;
	Eigen::MatrixXi inside_mask_;
	Eigen::VectorXf div_red_;
	Eigen::VectorXf div_green_ ;
	Eigen::VectorXf div_blue_ ;
};