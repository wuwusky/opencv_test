#include <opencv2/opencv.hpp>
#include <iostream>
//--------------------------------------------------------------------
//--------------------------------------------------------------------
using namespace std;
using namespace cv;
//--------------------------------------------------------------------
//--------------------------------------------------------------------
int main()
{
	Mat imgSrc = imread("4.bmp");
	imshow("ԭͼ", imgSrc);
	waitKey(0);

	vector<Mat>hsvChannels;
	split(imgSrc, hsvChannels);
	imshow("H", hsvChannels[0]);
	imshow("S", hsvChannels[1]);
	imshow("V", hsvChannels[2]);
	waitKey(0);
	vector<Mat>BGRchannels;
	split(imgSrc, BGRchannels);
	imshow("B", BGRchannels[0]);
	imshow("G", BGRchannels[1]);
	imshow("R", BGRchannels[2]);
	waitKey(0);
	equalizeHist(BGRchannels[0], BGRchannels[0]);
	equalizeHist(BGRchannels[1], BGRchannels[1]);
	equalizeHist(BGRchannels[2], BGRchannels[2]);

	Mat imgDstBGR;
	merge(BGRchannels, imgDstBGR);
	imshow("bgr", imgDstBGR);
	waitKey(0);

	equalizeHist(hsvChannels[0], hsvChannels[0]);
	equalizeHist(hsvChannels[1], hsvChannels[1]);
	equalizeHist(hsvChannels[2], hsvChannels[2]);
	Mat imgDstHSV;
	merge(hsvChannels, imgDstHSV);
	imshow("hsv", imgDstHSV);
	waitKey(0);

	return 1;
}