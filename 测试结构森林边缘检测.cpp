#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
#include <opencv2/line_descriptor.hpp>
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
int main()
{
	Mat imgSrc, imgDst;

	imgSrc = imread("..\\测试矫正\\test\\8最终结果.jpg");
	imshow("原图", imgSrc);
	waitKey(0);

	Mat imgTempForContour = imgSrc.clone();
	cvtColor(imgTempForContour, imgTempForContour, COLOR_BGR2GRAY);
	threshold(imgTempForContour, imgTempForContour, 0, 255, THRESH_OTSU);
	//imshow("OTSU", imgTempForContour);
	//waitKey(0);
	vector<vector<Point>> roiContours;
	findContours(imgTempForContour, roiContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	int iContourArea = 0;
	int iLabelContour;
	for (int i = 0; i < roiContours.size(); i++)
	{
		int temp = contourArea(roiContours[i]);
		if (iContourArea < temp)
		{
			iContourArea = temp;
			iLabelContour = i;
		}
	}
	Rect roiRect = boundingRect(roiContours[iLabelContour]);
	Mat imgRoi = imgSrc(roiRect);
	imshow("原图ROI", imgRoi);
	waitKey(0);
	Mat imgRoiFilter;
	bilateralFilter(imgRoi, imgRoiFilter, 5, 55, 25);
	imshow("滤波后", imgRoiFilter);
	waitKey(0);

	Mat imgGray;
	cvtColor(imgRoiFilter, imgGray, COLOR_BGR2GRAY);
	imshow("灰度图", imgGray);
	waitKey(0);
	Mat imgEdge;
	//Canny(imgGray, imgEdge, 50, 150);
	//imshow("canny", imgEdge);
	//waitKey(0);
	Sobel(imgGray, imgEdge, imgGray.depth(), 0, 3, 9, 2.0);
	imshow("sobel", imgEdge);
	waitKey(0);

	//Ptr<cv::ximgproc::StructuredEdgeDetection> detector = cv::ximgproc::createStructuredEdgeDetection("model.yml");
	Mat imgTemp = imgEdge.clone();
	//imgRoiFilter.convertTo(imgTemp, CV_32FC3, 1.0/255.0);
	//detector->detectEdges(imgTemp, imgDst);
	//imshow("结构森林", imgDst);
	//waitKey(0);
	////cvtColor(imgDst, imgTemp, COLOR_BGR2GRAY);
	//imgDst.convertTo(imgDst, CV_8UC1, 255.0);
	//threshold(imgDst, imgTemp, 0, 255, THRESH_OTSU);
	//imshow("结构二值", imgTemp);
	//waitKey(0);


	Mat kernelMat = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat kernelMatErode = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(imgEdge, imgTemp, MORPH_ERODE, kernelMatErode, Point(-1, -1), 1);
	//imshow("腐蚀", imgTemp);
	//waitKey(0);
	//morphologyEx(imgTemp, imgTemp, MORPH_DILATE, kernelMat, Point(-1,-1), 2);
	//imshow("膨胀下", imgTemp);
	//waitKey(0);

	Mat imgTempTemp = imgTemp.clone();
	cvtColor(imgTempTemp, imgTempTemp, COLOR_GRAY2BGR);
	Mat imgRoiMask = Mat::zeros(imgTemp.size(), CV_8UC1);
	for (int i = 0; i < imgRoiMask.rows; i++)
	{
		for (int j = 0; j < imgRoiMask.cols; j++)
		{
			if (j > imgRoiMask.cols / 10 && j < (imgRoiMask.cols / 10) * 9 && i > imgRoiMask.rows / 10 && i < (imgRoiMask.rows / 10) * 9)
			{
				imgRoiMask.at<uchar>(i, j) = 255;
			}
		}
	}
	imshow("RoiMask", imgRoiMask);
	waitKey(0);
	
	Ptr<cv::line_descriptor::LSDDetector> lineDetector = cv::line_descriptor::LSDDetector::createLSDDetector();
	vector<cv::line_descriptor::KeyLine> vecLines;
	lineDetector->detect(imgTemp, vecLines, 2, 3, imgRoiMask);
	//cout << "vecLines:" <<vecLines.size() << endl;
	vector<int> vecAngle;
	vecAngle.resize(361);

	for (int i = 0; i < vecLines.size(); i++)
	{
		if (vecLines[i].lineLength > 10)
		{
			int angle = (int)((vecLines[i].angle/CV_PI) * 180);
			vecAngle[angle + 180] ++;
		}
	}

	int iTemp = 0;
	int iLabelTemp = 0;
	for (int i = 0; i < vecAngle.size(); i++)
	{
		if (vecAngle[i] > iTemp)
		{
			iTemp = vecAngle[i];
			iLabelTemp = i - 180;
			cout << iLabelTemp <<"--" << iTemp <<endl;
		}
	}
	cout << "角度:" << iLabelTemp << endl;

	for (int i = 0; i < vecLines.size(); i++)
	{
		if ((int)((vecLines[i].angle/CV_PI) * 180) == iLabelTemp && vecLines[i].lineLength > 10)
		{
			line(imgTempTemp, Point(vecLines[i].startPointX, vecLines[i].startPointY), Point(vecLines[i].endPointX, vecLines[i].endPointY), Scalar(0, 255, 0), 2);
			//imshow("lines", imgTempTemp);
			//waitKey(0);
		}
	}

	imshow("lines", imgTempTemp);
	waitKey(0);

	Mat rotateMatAngle;
	int angleTest = iLabelTemp;
	if (angleTest == 0)
	{
		rotateMatAngle = getRotationMatrix2D(Point((imgTemp.cols / 2) - 1, (imgTemp.rows / 2) - 1), 0, 1);
	}
	else if(angleTest < 0)
	{
		if (abs(angleTest) <= 20)
		{
			rotateMatAngle = getRotationMatrix2D(Point((imgTemp.cols / 2) - 1, (imgTemp.rows / 2) - 1), -abs(angleTest), 1);
		}
		else
		{
			rotateMatAngle = getRotationMatrix2D(Point((imgTemp.cols / 2) - 1, (imgTemp.rows / 2) - 1), 180 - abs(angleTest), 1);
		}
	}
	else
	{ 
		if (abs(angleTest) <= 20)
		{
			rotateMatAngle = getRotationMatrix2D(Point((imgTemp.cols / 2) - 1, (imgTemp.rows / 2) - 1), -abs(angleTest), 1);
		}
		else
		{
			rotateMatAngle = getRotationMatrix2D(Point((imgTemp.cols / 2) - 1, (imgTemp.rows / 2) - 1), abs(angleTest) - 180, 1);
		}
	} 

	Mat imgRoiDst;
	warpAffine(imgRoi, imgRoiDst, rotateMatAngle, imgRoi.size());
	imshow("矫正后", imgRoiDst);
	waitKey(0);
	//return 0;

}