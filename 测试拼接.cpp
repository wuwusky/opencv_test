#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d.hpp"
#include <iostream>
#include "Header.h"
#include "gms_matcher.h"
#include "opencv2/reg/mapaffine.hpp"
#include "opencv2/reg/mapshift.hpp"
#include "opencv2/reg/mapprojec.hpp"
#include "opencv2/reg/mappergradshift.hpp"
#include "opencv2/reg/mappergradeuclid.hpp"
#include "opencv2/reg/mappergradsimilar.hpp"
#include "opencv2/reg/mappergradaffine.hpp"
#include "opencv2/reg/mappergradproj.hpp"
#include "opencv2/reg/mapperpyramid.hpp"

#include "opencv2/line_descriptor.hpp"
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void showDiff(Mat& image1, Mat& image2);

int main()
{
	int iFlag = 4;
	Mat imgSrc1 = imread("..//data//s1.jpg");
	Mat imgSrc2 = imread("..//data//s2.jpg");
	Mat imgG1, imgG2;
	Point p1, p2;
	p1 = Point(631, 839);
	p2 = Point(609, 103);


	//showDiff(imgSrc1, imgSrc2);

	Mat imgRoi1, imgRoi2;

	imgRoi1 = imgSrc1(Rect(0, p1.y - p2.y, imgSrc1.cols, (imgSrc1.rows - p1.y) + p2.y));
	imgRoi2 = imgSrc2(Rect(0, 0, imgSrc2.cols, (imgSrc1.rows - p1.y) + p2.y));

	//imshow("重复区域1", imgRoi1);
	//imshow("重复区域2", imgRoi2);
	//imwrite("imgr2.jpg", imgRoi2);
	//waitKey(0);

	showDiff(imgRoi1, imgRoi2);

	cvtColor(imgRoi1, imgG1, COLOR_BGR2GRAY);
	cvtColor(imgRoi2, imgG2, COLOR_BGR2GRAY);

	//Mat imgBin1, imgBin2;
	//threshold(imgG1, imgBin1, 0, 255, THRESH_OTSU);
	//imshow("bin", imgBin1);
	//waitKey(0);

	//Ptr<xfeatures2d::SURF> surfD = xfeatures2d::SURF::create(100, 4, 3, true, false);
	Ptr<cv::ORB> orbD = cv::ORB::create(10000);

	Mat mask1, mask2;
	//mask1 = Mat::zeros(imgSrc1.size(), CV_8UC1);
	//mask2 = Mat::zeros(imgSrc1.size(), CV_8UC1);
	//for (int i = 0; i < imgSrc1.rows; i++)
	//{
	//	for (int j = 0; j < imgSrc1.cols; j++)
	//	{
	//		if (i > p1.y)
	//		{
	//			mask1.at<uchar>(i, j) = 1;
	//		}
	//		if (i > p2.y && i < (imgSrc1.rows - p1.y))
	//		{
	//			mask2.at<uchar>(i, j) = 1;
	//		}
	//	}
	//}

	vector<KeyPoint> vecKP1, vecKP2;
	Mat mP1, mP2;
	orbD->detectAndCompute(imgG1, mask1, vecKP1, mP1);
	orbD->detectAndCompute(imgG2, mask2, vecKP2, mP2);


	Mat temp;
	//FlannBasedMatcher matcher;
	//BFMatcher matcher(NORM_L2);
	BFMatcher matcher(NORM_HAMMING);
	vector<DMatch> vecMatches;
	matcher.match(mP1, mP2, vecMatches);

	temp = DrawInlier(imgRoi1, imgRoi2, vecKP1, vecKP2, vecMatches, 2);

	imwrite("test.jpg", temp);

	switch (iFlag)
	{
	case 1:
	{
		{
			Mat HRansac;
			vector<int> vecInline;
			vector<Point> vecP1, vecP2;

			for (int i = 0; i < vecMatches.size(); i++)
			{
				vecP1.push_back(vecKP1[vecMatches[i].queryIdx].pt);
				vecP2.push_back(vecKP2[vecMatches[i].trainIdx].pt);
			}
			HRansac = findHomography(vecP1, vecP2, CV_RANSAC, 3.0, vecInline);

			vector<DMatch> vecInlineMatches;
			for (int i = 0; i < vecInline.size(); i++)
			{

				if (vecInline[i] > 0)
				{
					vecInlineMatches.push_back(vecMatches[i]);
				}
			}

			temp = DrawInlier(imgRoi1, imgRoi2, vecKP1, vecKP2, vecInlineMatches, 2);
			imwrite("ransac.jpg", temp);

			Mat imgDst;
			warpPerspective(imgRoi1, imgDst, HRansac, imgRoi2.size());
			warpPerspective(imgSrc1, temp, HRansac, imgSrc1.size());
			imwrite("warpALL.jpg", temp);
			imwrite("warp.jpg", imgDst);
			showDiff(imgDst, imgRoi2);



			Mat imgEnd1, imgEnd2;
			imgEnd1 = Mat::zeros(imgSrc1.size(), CV_8UC1);
			imgEnd2 = Mat::zeros(imgSrc2.size(), CV_8UC3);

			imgEnd1 = temp.clone();
			imgDst.copyTo(imgEnd1(Rect(0, imgEnd1.rows - imgDst.rows - 1, imgDst.cols, imgDst.rows)));
			imshow("imgEnd1", imgEnd1);
			waitKey(0);

		}
		break;
	}
	case 2:
	{

		{
			int num_inliers = 0;
			std::vector<bool> vecInliers;
			gms_matcher gms(vecKP1, imgRoi1.size(), vecKP2, imgRoi2.size(), vecMatches);
			num_inliers = gms.GetInlierMask(vecInliers, false, false);
			vector<DMatch> vecMatchesGMS;

			for (int i = 0; i < vecInliers.size(); i++)
			{
				if (vecInliers[i] == true)
				{
					vecMatchesGMS.push_back(vecMatches[i]);
				}
			}
			Mat imgDst = DrawInlier(imgRoi1, imgRoi2, vecKP1, vecKP2, vecMatchesGMS, 2);
			imwrite("gms.jpg", imgDst);
		}
		break;
	}
	case 3:
	{
		Ptr<cv::reg::MapperGradProj> mapper = makePtr<cv::reg::MapperGradProj>();
		cv::reg::MapperPyramid mappPyr(mapper);
		Ptr<cv::reg::Map> mapPtr = mappPyr.calculate(imgRoi1, imgRoi2);

		cv::reg::MapProjec* mapProj = dynamic_cast<cv::reg::MapProjec*>(mapPtr.get());
		mapProj->normalize();

		cout << Mat(mapProj->getProjTr()) << endl;
		Mat imgDst;
		mapProj->inverseWarp(imgRoi2, imgDst);
		showDiff(imgRoi1, imgDst);


		break;
	}
	case 4:
	{
		Ptr<line_descriptor::LSDDetector> lineD = line_descriptor::LSDDetector::createLSDDetector();
		vector<line_descriptor::KeyLine> vecKL1, vecKL2;
		Mat desL1, desL2;
		lineD->detect(imgRoi1, vecKL1, 1, 1);
		lineD->detect(imgRoi2, vecKL2, 1, 1);
		Ptr<line_descriptor::BinaryDescriptor> lineDes = line_descriptor::BinaryDescriptor::createBinaryDescriptor();
		lineDes->compute(imgRoi1, vecKL1, desL1);
		lineDes->compute(imgRoi2, vecKL2, desL2);

		std::vector<DMatch> line_matches;
		Ptr<line_descriptor::BinaryDescriptorMatcher> bdm = line_descriptor::BinaryDescriptorMatcher::createBinaryDescriptorMatcher();
		bdm->match(desL1, desL2, line_matches);

		std::vector<char> lsd_mask(line_matches.size(), 1);

		vector<DMatch> line_matches_good;
		for (int i=0;i< line_matches.size();i++)
		{
			if (line_matches[i].distance < 10)
			{
				line_matches_good.push_back(line_matches[i]);
			}

		}

		line_descriptor::drawLineMatches(imgRoi1, vecKL1, imgRoi2, vecKL2, line_matches_good, temp, Scalar(0, 255, 0), Scalar::all(-1), lsd_mask);
		imshow("line", temp);
		imwrite("line.jpg", temp);
		waitKey(0);
		
		break;
	}
	}



	
	return 1;
}

void showDiff(Mat& image1, Mat& image2)
{
	Mat img1, img2;
	image1.convertTo(img1, CV_32FC3);
	image2.convertTo(img2, CV_32FC3);

	if (img1.channels() != 1)
	{
		cvtColor(img1, img1, COLOR_BGR2GRAY);
	}
	if (img2.channels() != 1)
	{
		cvtColor(img2, img2, COLOR_BGR2GRAY);
	}

	Mat imgDiff;
	img1.copyTo(imgDiff);
	imgDiff = imgDiff - img2;
	imgDiff = imgDiff / 2.f;
	imgDiff = imgDiff + 128.f;

	Mat imgEnd;
	imgDiff.convertTo(imgEnd, CV_8UC3);
	imshow("diff", imgEnd);
	waitKey(0);
}