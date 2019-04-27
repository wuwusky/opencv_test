#include <opencv2/opencv.hpp>
#include <iostream>
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
using namespace std;
using namespace cv;
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int main()
{
	Mat imgSrc = imread("11.jpg");
	resize(imgSrc, imgSrc, Size(), 0.6, 0.6);
	imshow("src", imgSrc);
	Mat imgGray, imgBin;

	cvtColor(imgSrc, imgGray, COLOR_BGR2GRAY);
	threshold(imgGray, imgBin, 200, 255, THRESH_BINARY_INV);
	imshow("bin", imgBin);

	Mat kernel = getStructuringElement(MORPH_CROSS, Size(3, 3));
	Mat imgMorph;
	morphologyEx(imgBin, imgMorph, MORPH_DILATE, kernel);
	imshow("morph", imgMorph);

	vector<Vec4i>lines;
	HoughLinesP(imgMorph, lines, 1, CV_PI / 180, 200, 100, 5);
	Mat imgTemp;
	cvtColor(imgMorph, imgTemp, COLOR_GRAY2BGR);
	//= imgSrc.clone();

	int lineLengthLabel;
	double lineLength = 0.0;
	for (int i = 0; i < lines.size(); i++)
	{
		double temp = sqrt(abs((lines[i][0] - lines[i][2])*(lines[i][0] - lines[i][2])) + abs((lines[i][1] - lines[i][3])*(lines[i][1] - lines[i][3])));
		if (temp > lineLength)
		{
			lineLength = temp;
			lineLengthLabel = i;
		}
	}

	line(imgTemp, Point(lines[lineLengthLabel][0], lines[lineLengthLabel][1]), Point(lines[lineLengthLabel][2], lines[lineLengthLabel][3]), Scalar(0, 255, 0), 2);
	imshow("test", imgTemp);

	waitKey(0);
}

