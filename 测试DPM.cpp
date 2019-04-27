#include <opencv2/dpm.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <stdio.h>
#include <iostream>
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
using namespace cv;
using namespace cv::dpm;
using namespace std;
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void drawBoxes(Mat &frame, vector<DPMDetector::ObjectDetection> ds, Scalar color, string text);

int main()
{
	cv::Ptr<DPMDetector> detector = DPMDetector::create(vector<string>(1, "cat.xml"));
	vector<DPMDetector::ObjectDetection> dpmObjs;

	Mat srcImage = imread("cat.jpg",1);
	Mat dstImage = srcImage.clone();
	//cvtColor(srcImage, srcImage, COLOR_BGR2GRAY);
	//GaussianBlur(srcImage, srcImage, Size(5, 5), 3);

	detector->detect(srcImage, dpmObjs);
	drawBoxes(dstImage, dpmObjs, Scalar(0, 255, 0), "car");

	imshow("½á¹û",dstImage);
	waitKey(0);
	return 1;
}

void drawBoxes(Mat &frame, vector<DPMDetector::ObjectDetection> ds, Scalar color, string text)
{
	for (unsigned int i = 0; i < ds.size(); i++)
	{

		if (ds[i].score > -1)
		{
			rectangle(frame, ds[i].rect, color, 2);
		}
		
	}

	// draw text on image
	Scalar textColor(0,0,250);
	//putText(frame, text, Point(10,50), FONT_HERSHEY_PLAIN, 2, textColor, 2);
}
