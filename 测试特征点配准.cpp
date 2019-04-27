#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <Header.h>
#include <gms_matcher.h>
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
Point getDiff(vector<DMatch> vecInlineMatches, vector<cv::KeyPoint> vecP1, vector<cv::KeyPoint> vecP2, Mat imgS);
void DoubleMatchF(std::vector<DMatch> Match_Tmp2Det, std::vector<DMatch> Match_Det2Tmp, std::vector<DMatch> &Match_);

Point getDiff(vector<DMatch> vecInlineMatches, vector<cv::KeyPoint> vecP1, vector<cv::KeyPoint> vecP2, Mat imgS)
{
	vector<int> vecDx(imgS.cols*2);
	vector<int> vecDy(imgS.rows*2);

	for (int i=0;i< vecInlineMatches.size();i++)
	{
		double dx = vecP1[vecInlineMatches[i].queryIdx].pt.x - vecP2[vecInlineMatches[i].trainIdx].pt.x;
		double dy = vecP1[vecInlineMatches[i].queryIdx].pt.y - vecP2[vecInlineMatches[i].trainIdx].pt.y;
		int iDx = int(dx) + imgS.cols;
		int iDy = int(dy) + imgS.rows;

		if (iDx < vecDx.size() && iDy < vecDy.size())
		{
			vecDx[iDx]++;
			vecDy[iDy]++;
		}
	}
	int iLx = 0;
	int iLy = 0;
	int temp = 0;
	for (int i=0;i< vecDx.size();i++)
	{
		if (vecDx[i] > temp)
		{
			temp = vecDx[i];
			iLx = i;
		}
	}
	temp = 0;
	for (int i=0;i< vecDy.size();i++)
	{
		if (vecDy[i] > temp)
		{
			temp = vecDy[i];
			iLy = i;
		}
	}
	Point diffDst;
	diffDst = Point(iLx - imgS.cols, iLy - imgS.rows);
	return diffDst;
}

void matchGMS(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2, vector<DMatch>& vecInlineMatches)
{
	//Ptr<cv::ORB> orbD = cv::ORB::create(50000);
	Ptr<xfeatures2d::SURF> orbD = xfeatures2d::SURF::create(5, 4, 6, true, true);
	Ptr<cv::FastFeatureDetector> fastD = FastFeatureDetector::create(3);
	//Ptr<cv::xfeatures2d::FREAK> freakD = xfeatures2d::FREAK::create(false, false);

	cv::Mat imgDes1, imgDes2;
	//GaussianBlur(img1, img1, Size(3, 3), 5);
	//GaussianBlur(img2, img2, Size(3, 3), 5);
	Mat maskT;
	//maskT = imread("7_1.jpg", 0);
	orbD->detect(img1, vecP1, maskT);
	orbD->detect(img2, vecP2);
	//fastD->detect(img1, vecP1);
	//fastD->detect(img2, vecP2);

	vector<KeyPoint>vecP1new, vecP2new;
	if (vecP1.size() > 10000)
	{
		for (int i=0;i< 10000;i++)
		{
			vecP1new.push_back(vecP1[i]);
		}
	}
	else
	{
		vecP1new = vecP1;
	}
	if (vecP2.size() > 10000)
	{
		for (int i = 0; i < 10000; i++)
		{
			vecP2new.push_back(vecP2[i]);
		}
	}
	else
	{
		vecP2new = vecP2;
	}
	orbD->compute(img1, vecP1new, imgDes1);
	orbD->compute(img2, vecP2new, imgDes2);
	//fastD->detect(img1, vecP1);
	//fastD->detect(img2, vecP2);
	//freakD->compute(img1, vecP1, imgDes1);
	//freakD->compute(img2, vecP2, imgDes2);

	if (vecP2new.size() < 4)
	{
		vecP2new = vecP1new;
		imgDes2 = imgDes1;
	}
	vector<DMatch> vecMatches;
	//vector<DMatch> vecMatches2;

	//BFMatcher match1(NORM_HAMMING);
	FlannBasedMatcher match1;

	match1.match(imgDes1, imgDes2, vecMatches);
	//match1.match(imgDes2, imgDes1, vecMatches2);

	//Mat imgDst;
	//drawMatches(img1, vecP1, img2, vecP2, vecMatches, imgDst, Scalar(0, 255, 255), Scalar(0, 255, 255));
	//resize(imgDst, imgDst, Size(960, 320));
	//imshow("match1", imgDst);
	//waitKey(0);

	//vector<DMatch> vecMatcheGms;
	vector<bool> vecInliners;
	gms_matcher gms(vecP1new, img1.size(), vecP2new, img2.size(), vecMatches);
	int num_inliners = gms.GetInlierMask(vecInliners, false, false);
	for (int i = 0; i < vecInliners.size(); i++)
	{

		if (vecInliners[i])
		{
			vecInlineMatches.push_back(vecMatches[i]);
		}
	}

	cvtColor(img1, img1, COLOR_GRAY2BGR);
	cvtColor(img2, img2, COLOR_GRAY2BGR);
	Mat show = DrawInlier(img1, img2, vecP1new, vecP2new, vecInlineMatches, 2);
	resize(show, show, Size(960, 320));
	imshow("gms", show);

	//-----------test---------------------------------
	vector<Point> vecP1Test, vecP2Test;
	for (int i = 0; i < vecInlineMatches.size(); i++)
	{
		vecP1Test.push_back(vecP1new[vecInlineMatches[i].queryIdx].pt);
		vecP2Test.push_back(vecP2new[vecInlineMatches[i].trainIdx].pt);
	}
	vector<int> vecMask;
	Mat HTest = findHomography(vecP1Test, vecP2Test, vecMask, LMEDS, 0);
	vector<DMatch> vecMatchesTest;
	for (int i = 0; i < vecMask.size(); i++)
	{
		if (vecMask[i])
		{
			vecMatchesTest.push_back(vecInlineMatches[i]);
		}
	}
	Mat show1 = DrawInlier(img1, img2, vecP1new, vecP2new, vecMatchesTest, 2);
	resize(show1, show1, Size(960, 320));
	imshow("gms+ransac", show1);


}

void matchMath(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2, vector<DMatch>& vecInlineMatches)
{
	Ptr<xfeatures2d::SURF> orbD = xfeatures2d::SURF::create(5, 4, 6, true, true);
	cv::Mat imgDes1, imgDes2;
	Mat maskT;
	//maskT = imread("7_1.jpg", 0);
	orbD->detect(img1, vecP1, maskT);

	orbD->detect(img2, vecP2);
	vector<KeyPoint>vecP1new, vecP2new;
	if (vecP1.size() > 10000)
	{
		for (int i = 0; i < 10000; i++)
		{
			vecP1new.push_back(vecP1[i]);
		}
	}
	else
	{
		vecP1new = vecP1;
	}
	if (vecP2.size() > 10000)
	{
		for (int i = 0; i < 10000; i++)
		{
			vecP2new.push_back(vecP2[i]);
		}
	}
	else
	{
		vecP2new = vecP2;
	}
	orbD->compute(img1, vecP1new, imgDes1);
	orbD->compute(img2, vecP2new, imgDes2);
	if (vecP2new.size() < 4)
	{
		vecP2new = vecP1new;
		imgDes2 = imgDes1;
	}
	vector<DMatch> vecMatches1;
	vector<DMatch> vecMatches2;

	FlannBasedMatcher match1;

	match1.match(imgDes1, imgDes2, vecMatches1);
	match1.match(imgDes2, imgDes1, vecMatches2);

	DoubleMatchF(vecMatches1, vecMatches2, vecInlineMatches);

	cvtColor(img1, img1, COLOR_GRAY2BGR);
	cvtColor(img2, img2, COLOR_GRAY2BGR);
	Mat show = DrawInlier(img1, img2, vecP1new, vecP2new, vecInlineMatches, 2);
	resize(show, show, Size(960, 320));
	imshow("double", show);
	//-----------test---------------------------------
	vector<Point> vecP1Test, vecP2Test;
	for (int i = 0; i < vecInlineMatches.size(); i++)
	{
		vecP1Test.push_back(vecP1new[vecInlineMatches[i].queryIdx].pt);
		vecP2Test.push_back(vecP2new[vecInlineMatches[i].trainIdx].pt);
	}
	vector<int> vecMask;
	//Mat HTest = findHomography(vecP1Test, vecP2Test, CV_RANSAC, 3, vecMask, 2000);
	//Mat HTest = findHomography(vecP1Test, vecP2Test, vecMask, LMEDS, 0);

	vector<DMatch> vecMatchesTest;
	for (int i = 0; i < vecMask.size(); i++)
	{
		if (vecMask[i])
		{
			vecMatchesTest.push_back(vecInlineMatches[i]);
		}
	}
	Mat show1 = DrawInlier(img1, img2, vecP1new, vecP2new, vecMatchesTest, 2);
	resize(show1, show1, Size(960, 320));
	imshow("double+ransac", show1);

}


void test1()
{
	cv::Mat img1, img2;

	//img1 = imread("..//油位计检测结果//roi//00011_0002.jpg", 0);
	//img2 = imread("..//油位计检测结果//roi//00011_0029.jpg", 0);
	//img1 = imread("00016_0001.jpg", 0);
	//img2 = imread("00016_0021.jpg", 0);
	img1 = imread("5.jpg", 0);
	img2 = imread("6.jpg", 0);
	Mat img1S, img2S;
	resize(img1, img1S, Size(), 0.5, 0.5);
	resize(img2, img2S, Size(), 0.5, 0.5);

	vector<cv::KeyPoint> vecP1, vecP2;
	vector<DMatch> vecInlineMatches;
	vector<DMatch> vecInlineMatches2;
	vector<DMatch> vecInlineMatches3;
	//matchMath(img1, img2, vecP1, vecP2, vecInlineMatches2);
	//Point testDiff2 = getDiff(vecInlineMatches2, vecP1, vecP2, img1);
	//cout << "DM:" << testDiff2 << endl;

	matchGMS(img1, img2, vecP1, vecP2, vecInlineMatches);
	Point testDiff = getDiff(vecInlineMatches, vecP1, vecP2, img1);
	cout << "GMS:" << testDiff << endl;

	//matchMath(img1, img2, vecP1, vecP2, vecInlineMatches2);
	//matchGMS(img1, img2, vecP1, vecP2, vecInlineMatches);
	//matchGMS(img1S, img2S, vecP1, vecP2, vecInlineMatches3);
	//Point testDiff3 = getDiff(vecInlineMatches3, vecP1, vecP2, img1S);
	//testDiff3.x = testDiff3.x * 2;
	//testDiff3.y = testDiff3.y * 2;
	//cout << "Scale:" << testDiff3 << endl;


	cout << "sizeGMS:" << vecInlineMatches.size() << endl;
	cout << "sizeDM:" << vecInlineMatches2.size() << endl;


	//resize(img1, img1, Size(480, 320));
	//resize(img2, img2, Size(480, 320));


	waitKey(0);

}


int main()
{

	test1();
	//cv::Mat img1, img2;
	//cout << "开始" << endl;

	//img1 = imread("1.jpg", 0);
	//img2 = imread("1.jpg", 0);


	//Ptr<cv::xfeatures2d::SurfFeatureDetector> surfD = cv::xfeatures2d::SurfFeatureDetector::create(100, 4, 2, false, false);
	//Ptr<cv::xfeatures2d::SurfDescriptorExtractor> surfE = cv::xfeatures2d::SurfDescriptorExtractor::create(100, 4, 2, false, false);

	//vector<cv::KeyPoint> vecP1, vecP2;
	//cv::Mat imgDes1, imgDes2;

	//surfD->detect(img1, vecP1);
	//surfD->detect(img2, vecP2);

	//vector<cv::KeyPoint> vecP3;
	//for (int i=0;i< 5000;i++)
	//{
	//	vecP3.push_back(vecP2[i]);
	//}

	//surfD->compute(img1, vecP1, imgDes1);
	//surfD->compute(img2, vecP3, imgDes2);

	//vector<DMatch> vecMatches;
	//vector<DMatch> vecMatches2;

	//BFMatcher match1(NORM_L2);
	////FlannBasedMatcher match1;

	//match1.match(imgDes1, imgDes2, vecMatches);
	//match1.match(imgDes2, imgDes1, vecMatches2);

	//cout << "结束" << endl;


	//vector<DMatch> vecMatcheGms;
	//vector<bool> vecInliners;
	//gms_matcher gms(vecP1, img1.size(), vecP2, img2.size(), vecMatcheGms);
	//int num_inliners = gms.GetInlierMask(vecInliners, false, false);

	return 1;
}



void DoubleMatchF(std::vector<DMatch> Match_Tmp2Det, std::vector<DMatch> Match_Det2Tmp, std::vector<DMatch> &Match_)
{
	int l = Match_Tmp2Det.size();
	int r = Match_Det2Tmp.size();

	int i, j;
	for (i = 0; i < l; i++)
	{
		int one = Match_Tmp2Det[i].queryIdx;
		int two = Match_Tmp2Det[i].trainIdx;

		for (j = 0; j < r; j++)
		{
			int one1 = Match_Det2Tmp[j].queryIdx;
			int two1 = Match_Det2Tmp[j].trainIdx;

			if (one == two1 && two == one1)
			{
				Match_.push_back(Match_Tmp2Det[i]);
				break;
			}
		}
	}
}
//CvPoint SimpleReg(std::vector<Point2f> scene, std::vector<Point2f> obj, int iImgWidth, int iImgHeight)
//{
//	if (scene.size() <= 0 || obj.size() <= 0 || iImgWidth <= 0 || iImgHeight <= 0)
//	{
//		return cvPoint(0, 0);
//	}
//
//	if (CV_IS_IMAGE(m_pIplDiff))
//	{
//		if (!(m_pIplDiff->width == iImgWidth * 2 && m_pIplDiff->height == iImgHeight * 2))
//		{
//			cvReleaseImage(&m_pIplDiff);
//			m_pIplDiff = cvCreateImage(cvSize(iImgWidth * 2, iImgHeight * 2), IPL_DEPTH_32F, 1);
//		}
//	}
//	else
//	{
//		m_pIplDiff = cvCreateImage(cvSize(iImgWidth * 2, iImgHeight * 2), IPL_DEPTH_32F, 1);
//	}
//
//	cvZero(m_pIplDiff);
//
//	int dx = 0;
//	int dy = 0;
//	double dMaxValue = 0.0;
//	CvPoint CvPoint_MaxLoc;
//
//	for (int i = 0; i < scene.size(); i++)
//	{
//		dx = scene[i].x - obj[i].x + iImgWidth;
//		dy = scene[i].y - obj[i].y + iImgHeight;
//		CV_IMAGE_ELEM(m_pIplDiff, float, dy, dx) += 1;
//	}
//
//	cvSmooth(m_pIplDiff, m_pIplDiff, CV_GAUSSIAN, 3, 0, 0, 0);
//	cvNormalize(m_pIplDiff, m_pIplDiff, 255, 0, CV_L2, NULL);
//	cvMinMaxLoc(m_pIplDiff, NULL, &dMaxValue, NULL, &CvPoint_MaxLoc, 0);
//
//	CvPoint_MaxLoc.x -= iImgWidth;
//	CvPoint_MaxLoc.x = -CvPoint_MaxLoc.x;
//	CvPoint_MaxLoc.y -= iImgHeight;
//	CvPoint_MaxLoc.y = -CvPoint_MaxLoc.y;
//
//	return CvPoint_MaxLoc;
//}
