#include <opencv2/opencv.hpp>
#include <opencv2/viz.hpp>
#include <iostream>
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//int main()
//{
//	viz::Viz3d testWindow("µ„‘∆");
//	//testWindow.setBackgroundColor();
//	
//	cv::Mat imgSrc = imread("1.jpg");
//	//testWindow.showImage(imgSrc, Size(600, 400));
//	testWindow.setBackgroundMeshLab();
//	testWindow.showWidget("◊¯±Í", cv::viz::WCoordinateSystem());
//	while (!testWindow.wasStopped())
//	{
//		testWindow.spin();
//		break;
//	}
//
//
//	return 1;
//}

int main()
{
	cv::Mat img = imread("1.jpg");
	CvPoint pDiff = CvPoint(100, 100);

	cv::Mat imgDst = img.clone();
	imgDst = Mat::zeros(imgDst.size(), CV_8UC3);

	cv::Rect rect;
	cv::Mat mTemp;
	if (pDiff.x >= 0 && pDiff.y >= 0)
	{
		rect.x = 0;
		rect.y = 0;
		rect.width = img.cols - pDiff.x;
		rect.height = img.rows - pDiff.y;
		mTemp = img(rect);

		rect.x = pDiff.x;
		rect.y = pDiff.y;
		rect.width = img.cols - pDiff.x;
		rect.height = img.rows - pDiff.y;

		mTemp.copyTo(imgDst(rect));
	}

	imshow("≤‚ ‘", imgDst);
	waitKey(0);

}