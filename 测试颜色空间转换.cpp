#include <opencv2/opencv.hpp>
#include <iostream>
//-----------------------------------------------------------------
//-----------------------------------------------------------------
using namespace std;
using namespace cv;
//-----------------------------------------------------------------
//-----------------------------------------------------------------
int main()
{
	
	cv::Mat imgSrc;
	imgSrc = imread("C:\\Users\\wuwuw\\Desktop\\专利相关\\数字表专利\\image\\11.jpg");
	cv::Mat imgDstHSV, imgDstLab;


	cvtColor(imgSrc, imgDstLab, COLOR_BGR2Lab);
	cvtColor(imgSrc, imgDstHSV, COLOR_BGR2HSV_FULL);
	imshow("src", imgSrc);
	imshow("Lab", imgDstLab);
	imshow("HSV", imgDstHSV);

	vector<cv::Mat> mChannels;

	split(imgDstLab, mChannels);

	equalizeHist(mChannels[1], mChannels[1]);
	equalizeHist(mChannels[2], mChannels[2]);
	imshow("a", mChannels[1]);
	imshow("b", mChannels[2]);

	waitKey(0);
}