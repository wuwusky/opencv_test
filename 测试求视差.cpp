#include "opencv2/stereo.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv/cvaux.hpp"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <opencv2/viz.hpp>
//--------------------------------------------------------//
//--------------------------------------------------------//
using namespace std;
using namespace cv;
using namespace cv::stereo;
//--------------------------------------------------------//
//--------------------------------------------------------//
int main()
{
	Mat left = imread("imL2l.bmp", 0);
	Mat right = imread("imL2.bmp", 0);
	Mat dst;
	double minV, maxV;

	resize(left, left, Size(), 1, 1);
	resize(right, right, Size(), 1, 1);

	double t1 = getTickCount();
	Ptr<StereoBM> sBM = StereoBM::create();
	sBM->setPreFilterType(CV_STEREO_BM_NORMALIZED_RESPONSE);//预处理滤波器的类型，两种可选：CV_STEREO_BM_NORMALIZED_RESPONSE或者CV_STEREO_BM_XSOBEL
	sBM->setBlockSize(15);//SAD窗口大小，一般在5*5到21*21之间，必须奇数
	sBM->setUniquenessRatio(15);//视差唯一性百分比，一般5-15之间
	sBM->setNumDisparities(32);//视差窗口，最大视差值和最小视差值的差，大小必须是16的整数倍
	sBM->compute(left, right, dst);
	minMaxLoc(dst, &minV, &maxV);
	dst.convertTo(dst, CV_8UC1, 255/(maxV - minV));

	double t2 = getTickCount();
	double t = (double)(t2 - t1)/getTickFrequency();
	//cout << dst <<endl;
	imshow("src1", left);
	imshow("src2", right);
	imshow("BM", dst);

	cout << "BM算法耗时："<< t <<endl;
	//waitKey(0);
	//-------------------------------------------------------------------------------------
	t1 = getTickCount();

	Ptr<StereoSGBM> sbbm = StereoSGBM::create(0, 16, 5);
	sbbm->setNumDisparities(32);////视差窗口，最大视差值和最小视差值的差，大小必须是16的整数倍
	sbbm->setP1(484);//控制视差变化平滑性的参数，相邻像素点视差增减1时的惩罚系数
	sbbm->setP2(3872);//控制视差变化平滑性的参数，相邻像素点视差变化值大于1时的惩罚系数，P2必须大于P1
	sbbm->setBlockSize(11);//SAD窗口大小，一般3*3到11*11之间，必须是奇数
	sbbm->compute(left, right, dst);
	minMaxLoc(dst, &minV, &maxV);
	dst.convertTo(dst, CV_8UC1, 255/(maxV - minV));

	t2 = getTickCount();
	t = (double)(t2 - t1)/getTickFrequency();
	cout << "SGBM算法耗时："<< t <<endl;
	//cout << dst <<endl;
	imshow("SGBM", dst);
	waitKey(0);
	//-------------------------------------------------------------------------------------



	return 0;
}

