#include <opencv2/opencv.hpp>
#include <iostream>
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
void gaussianSmooth(Mat src, vector<double>kernel, Mat& dst);
void normalizeMy(Mat src, Mat &dst);
//-----------------------------------------------------------------------------------------
int main()
{
	cv::Mat imgSrc, imgDst;
	imgSrc = imread("3.jpg");

	cv::Mat imgSrcLab;
	cvtColor(imgSrc, imgSrcLab, COLOR_BGR2Lab);
	vector<cv::Mat> mChannels;
	split(imgSrcLab, mChannels);
	double avgl(0), avga(0), avgb(0);

	for (int i = 0; i < imgSrc.rows; i++)
	{
		for (int j = 0; j < imgSrc.cols; j++)
		{
			avgl += mChannels[0].at<uchar>(i, j);
			avga += mChannels[1].at<uchar>(i, j);
			avgb += mChannels[2].at<uchar>(i, j);
		}
	}

	avgl = avgl / (imgSrc.rows*imgSrc.cols); 
	avga = avga / (imgSrc.rows*imgSrc.cols);
	avgb = avgb / (imgSrc.rows*imgSrc.cols);


	vector<double> kernel(0);
	kernel.push_back(1.0);
	kernel.push_back(4.0);
	kernel.push_back(6.0);
	kernel.push_back(4.0);
	kernel.push_back(1.0);

	//kernel.push_back(1.0);
	//kernel.push_back(2.0);
	//kernel.push_back(1.0);

	cv::Mat imgL, imgA, imgB;
	gaussianSmooth(mChannels[0], kernel, imgL);
	gaussianSmooth(mChannels[1], kernel, imgA);
	gaussianSmooth(mChannels[2], kernel, imgB);

	Mat dstTemp;
	dstTemp.create(imgSrc.size(), CV_64FC1);

	for (int i = 0; i < imgSrc.rows; i++)
	{
		for (int j = 0; j < imgSrc.cols; j++)
		{
			dstTemp.at<double>(i, j) = (imgL.at<double>(i, j) - avgl) * (imgL.at<double>(i, j) - avgl) +
				(imgA.at<double>(i, j) - avga) * (imgA.at<double>(i, j) - avga) +
				(imgB.at<double>(i, j) - avgb) * (imgB.at<double>(i, j) - avgb);
		}
	}
	

	normalizeMy(dstTemp, imgDst);

	imshow("dst", imgDst);
	waitKey(0);
	return 0;

}

void gaussianSmooth(Mat src, vector<double>kernel, Mat& dst)
{
	int center = int(kernel.size()) / 2;
	cv::Mat tempSrc = cv::Mat::zeros(src.size(), CV_64FC1);

	//x方向
	for (int r = 0; r < src.rows; r++)
	{
		for (int c = 0; c < src.cols; c++)
		{
			double kernelsum(0);
			double sum(0);
			for (int cc = (-center); cc <= center; cc++)
			{
				if (((c + cc) >= 0) && ((c + cc) < src.cols))
				{
					sum += (src.at<uchar>(r, c+cc) * kernel[center + cc]);
					kernelsum += kernel[center + cc];
				}
			}
			tempSrc.at<double>(r, c) = (double)(sum / kernelsum);
		}
	}

	dst = tempSrc.clone();
	//y方向
	for (int r = 0; r < src.rows; r++)
	{
		for (int c = 0; c < src.cols; c++)
		{
			double kernelsum(0);
			double sum(0);
			for (int rr = (-center); rr <= center; rr++)
			{
				if (((r + rr) >= 0) && ((r + rr) < src.rows))
				{
					sum += (tempSrc.at<double>(r + rr, c) * kernel[center + rr]);
					kernelsum += kernel[center + rr];
				}
			}
			dst.at<double>(r, c) = sum / kernelsum;
		}
	}
}

void normalizeMy(Mat src, Mat &dst)
{
	double dMax = 0;
	double dMin = DBL_MAX;

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src.at<double>(i, j) > dMax)
			{
				dMax = src.at<double>(i, j);
			}
			if (src.at<double>(i, j) < dMin)
			{
				dMin = src.at<double>(i, j);
			}
		}
	}

	double dRange = dMax - dMin;
	dst.create(src.size(), CV_8UC1);
	for (int i = 0; i < dst.rows; i++)
	{
		for (int j = 0; j < dst.cols; j++)
		{
			dst.at<uchar>(i, j) = (uchar)(((src.at<double>(i, j) - dMin) / dRange) * 255);
		}
	}

}
