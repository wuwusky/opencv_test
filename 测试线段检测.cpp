#include <opencv2/opencv.hpp>
#include "opencv2/line_descriptor.hpp"
//#include <opencv2/line_descriptor/descriptor.hpp>
#include <iostream>
//---------------------------------------------------------------------//
//---------------------------------------------------------------------//
using namespace std;
using namespace cv;
using namespace line_descriptor;
//---------------------------------------------------------------------//
//---------------------------------------------------------------------//
int main ()
{
	Ptr<LSDDetector>  lineDetector = LSDDetector::createLSDDetector();

	Ptr<BinaryDescriptor> lineDetector1 = BinaryDescriptor::createBinaryDescriptor();

	Mat srcImage;
	vector<KeyLine> keylines, keylinesTemp;
	int scale = 0.5;
	int numOctaves = 1 ;

	srcImage = imread("3.jpg",0);
	Mat tempImage = imread("3.jpg",0);
	//threshold(srcImage, srcImage, 0, 255, THRESH_OTSU);
	//threshold(tempImage, tempImage, 0, 255, THRESH_OTSU);
	//lineDetector1->detect(srcImage, keylines);
	//lineDetector1->detect(tempImage, keylinesTemp);
	lineDetector->detect(srcImage,keylines,scale,numOctaves);
	lineDetector->detect(tempImage, keylinesTemp, scale, numOctaves);
	for (int i = 0; i < keylines.size(); i ++)
	{
		line(srcImage, Point(keylines[i].startPointX, keylines[i].startPointY), Point(keylines[i].endPointX, keylines[i].endPointY), Scalar(0, 255, 0), 2);
	}
	imshow("test",srcImage);
	waitKey(0);
	Mat srcDes,tempDes;
	lineDetector1->compute(srcImage, keylines, srcDes);
	lineDetector1->compute(tempImage, keylinesTemp, tempDes);

	Ptr<BinaryDescriptorMatcher> desMatch = BinaryDescriptorMatcher::createBinaryDescriptorMatcher();

	std::vector<vector<DMatch>> matches;
	desMatch->knnMatch(srcDes, tempDes, matches, 2);
	std::vector<DMatch>goodMatches;

	for (int i = 0; i < matches.size(); i ++)
	{
			if (matches[i][0].distance < 3 * matches[i][1].distance || matches[i][0].distance == 0)
			{
				goodMatches.push_back(matches[i][0]);
			}
	}
	cvtColor(srcImage, srcImage, COLOR_GRAY2BGR);
	cvtColor(tempImage, tempImage, COLOR_GRAY2BGR);

	Mat outImage;
	std::vector<char> mask( matches.size(), 1 );
	drawLineMatches(srcImage, keylines, tempImage, keylinesTemp, goodMatches, outImage, Scalar(0, 0, 255), Scalar(0, 255, 0), mask, DrawLinesMatchesFlags::DEFAULT);

	imshow("≤‚ ‘œ¬",outImage);
	waitKey(0);
	return 1;
}