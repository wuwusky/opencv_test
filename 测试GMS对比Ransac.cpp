#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <io.h>
#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/ximgproc.hpp>
#include <Header.h>
#include <gms_matcher.h>
#include "伺服.h"
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
using namespace std;
using namespace cv;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void getFiles(string path, vector<string>& files);
Point getDiff(vector<DMatch> vecInlineMatches, vector<cv::KeyPoint> vecP1, vector<cv::KeyPoint> vecP2, Mat imgS);
Mat matchGMS(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2, vector<DMatch>& vecInlineMatches);
Mat matchRansac(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2);
void getFiles(string path, vector<string>& files, vector<string>& fileNames);
Mat matchGMS(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2, Mat imgMask);
Mat getImgMask(Rect rectTmp, Mat imgTmp);


bool FlagIsShow = false;

Rect myRoi;
Mat imgFrame;
string imgCurrentAddr;
bool flagStart = false;
bool flagEnd = false;
//----------------------------------------------------------------------------------
static void myOnMouse(int event, int x, int y, int, void*)
{
	switch (event)
	{
	case EVENT_MOUSEMOVE:
	{
		if (flagStart && !flagEnd)
		{
			Mat temp;
			temp = imgFrame.clone();
			//imgFrame = imread(imgCurrentAddr);
			rectangle(temp, Point(myRoi.x, myRoi.y), Point(x, y), Scalar(0, 0, 255), 2);
			imshow("模板", temp);
		}

		break;
	}
	case EVENT_LBUTTONDOWN:
	{
		flagStart = true;
		myRoi.x = x;
		myRoi.y = y;
		flagEnd = false;

		break;
	}
	case EVENT_LBUTTONUP:
	{
		myRoi.width = abs(x - myRoi.x);
		myRoi.height = abs(y - myRoi.y);
		flagEnd = true;
		break;

	}
	}

}

void  test1()
{
	char* fileAddr = "..\\光州测试图片//model";
	vector <string> vecFilesAddr, vecFileNames;

	getFiles(fileAddr, vecFilesAddr, vecFileNames);

	fstream wr("..\\光州测试图片//model//标定信息.txt", ios::out);

	for (int i = 0; i < vecFilesAddr.size(); i++)
	{
		namedWindow("模板", 1);
		Mat imgSrc = imread(vecFilesAddr[i]);
		//resize(imgSrc, imgSrc, Size(), 0.5, 0.5);
		imgFrame = imgSrc.clone();
		//imshow("模板", imgSrc);
		setMouseCallback("模板", myOnMouse, NULL);
		imshow("模板", imgFrame);
		waitKey(0);

		string tempName;
		char* nameEnd = strchr((char*)(vecFileNames[i].c_str()), '_');
		char* nameStart = (char*)vecFileNames[i].c_str();
		tempName = vecFileNames[i].substr(0, (size_t)(nameEnd - nameStart));

		wr << tempName << " " << myRoi.x << " " << myRoi.y << " " << myRoi.width << " " << myRoi.height << endl;
		nameEnd = NULL;
		nameStart = NULL;
	}
	wr.close();
}

void getFiles(string path, vector<string>& files, vector<string>& fileNames)
{
	//文件句柄
	long hFile = 0;
	//文件的信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录，就进行迭代
			//如果不是目录，就加入文件列表files
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files, fileNames);
				}
			}
			else
			{
				fileNames.push_back(fileinfo.name);
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

int getDataLabel(string fileName, string labelStopFlage)//根据文件名得到样本的标签序号
{
	string file = fileName;
	string label[20];

	for (int i = 0; i < file.size(); i++)
	{
		if (file.substr(i, 1).compare(labelStopFlage))
		{
			label[i] = file.substr(i, 1);
		}
		else
		{
			break;
		}
	}

	std::stringstream temp;
	int labelTrainData;

	temp << label[0] + label[1];
	temp >> labelTrainData;

	return labelTrainData;
}

void getFiles(string path, vector<string>& files)
{
	//文件句柄
	long hFile = 0;
	//文件的信息
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录，就进行迭代
			//如果不是目录，就加入文件列表files
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
Point getDiff(vector<DMatch> vecInlineMatches, vector<cv::KeyPoint> vecP1, vector<cv::KeyPoint> vecP2, Mat imgS)
{
	vector<int> vecDx(imgS.cols * 2 + 2);
	vector<int> vecDy(imgS.rows * 2 + 2);
	//for (int i = 0; i < vecDx.size(); i++)
	//{
	//	vecDx[i] = 0;
	//}
	//for (int i = 0; i < vecDy.size(); i++)
	//{
	//	vecDy[i] = 0;
	//}

	for (int i = 0; i < vecInlineMatches.size(); i++)
	{
		if (i == 456)
		{
			int k = 1;
		}
		double dx = vecP1[vecInlineMatches[i].queryIdx].pt.x - vecP2[vecInlineMatches[i].trainIdx].pt.x;
		double dy = vecP1[vecInlineMatches[i].queryIdx].pt.y - vecP2[vecInlineMatches[i].trainIdx].pt.y;
		int iDx = int(dx) + imgS.cols;
		int iDy = int(dy) + imgS.rows;
		if (iDx >= 0 && iDy >= 0)
		{
			vecDx[iDx]++;
			vecDy[iDy]++;

		}
		//cout << i << endl;
		//cout << iDx << "-" << iDy << endl;
	}
	int iLx = 0;
	int iLy = 0;
	int temp = 0;
	for (int i = 0; i < vecDx.size(); i++)
	{
		if (vecDx[i] > temp)
		{
			temp = vecDx[i];
			iLx = i;
		}
	}
	temp = 0;
	for (int i = 0; i < vecDy.size(); i++)
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
Point getDiffNew(vector<DMatch> vecInlineMatches, vector<cv::KeyPoint> vecP1, vector<cv::KeyPoint> vecP2, Mat imgS)
{
	vector<Point2f> vecPT1, vecPT2;
	for (int i = 0; i < vecInlineMatches.size(); i++)
	{
		vecPT1.push_back(vecP1[vecInlineMatches[i].queryIdx].pt);
		vecPT2.push_back(vecP2[vecInlineMatches[i].trainIdx].pt);

	}

	Point diffDst;
	diffDst = SimpleReg(vecPT2, vecPT1, imgS.cols, imgS.rows);

	return diffDst;
}
Mat matchGMS(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2, Mat imgMask)
{
	Ptr<xfeatures2d::SURF> orbD = xfeatures2d::SURF::create(5, 4, 6, true, true);
	Ptr<cv::FastFeatureDetector> fastD = FastFeatureDetector::create(3);

	cv::Mat imgDes1, imgDes2;
	Mat maskT;
	//maskT = imread("..//光州测试图片//model//4.jpg", 0);
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

	//vector<DMatch> vecMatcheGms;
	vector<bool> vecInliners;
	vector<DMatch> vecInlineMatches;
	gms_matcher gms(vecP1new, img1.size(), vecP2new, img2.size(), vecMatches);
	int num_inliners = gms.GetInlierMask(vecInliners, false, false);
	for (int i = 0; i < vecInliners.size(); i++)
	{

		if (vecInliners[i])
		{
			vecInlineMatches.push_back(vecMatches[i]);
		}
	}

	if (FlagIsShow)
	{
		cout << "gmsSize:" << num_inliners << endl;
		cvtColor(img1, img1, COLOR_GRAY2BGR);
		cvtColor(img2, img2, COLOR_GRAY2BGR);
		Mat show = DrawInlier(img1, img2, vecP1new, vecP2new, vecInlineMatches, 2);
		resize(show, show, Size(960, 320));
		imshow("gms", show);
		//waitKey(0);

	}
	//-----------test---------------------------------
	vector<Point> vecP1Test, vecP2Test;
	for (int i = 0; i < vecInlineMatches.size(); i++)
	{
		vecP1Test.push_back(vecP1new[vecInlineMatches[i].queryIdx].pt);
		vecP2Test.push_back(vecP2new[vecInlineMatches[i].trainIdx].pt);
	}
	vector<int> vecMask;
	Mat HTest;
	if (vecInlineMatches.size() > 4)
	{
		HTest = findHomography(vecP2Test, vecP1Test, vecMask, LMEDS);
	}
	vector<DMatch> vecMatchesTest;
	for (int i = 0; i < vecMask.size(); i++)
	{
		if (vecMask[i])
		{
			vecMatchesTest.push_back(vecInlineMatches[i]);
		}
	}

	vecMatchesTest.swap(vecInlineMatches);

	if (FlagIsShow)
	{
		Mat show1 = DrawInlier(img1, img2, vecP1new, vecP2new, vecInlineMatches, 2);
		resize(show1, show1, Size(960, 320));
		imshow("gms+ransac", show1);
		waitKey(0);
	}

	return HTest;
}
Mat getImgMask(Rect rectTmp, Mat imgTmp)
{
	Mat imgMask = Mat::zeros(imgTmp.size(), CV_8UC1);
	Point pCenter = Point(rectTmp.x + rectTmp.width / 2, rectTmp.y + rectTmp.height / 2);
	int up, down, left, right;
	int w = rectTmp.width;
	int h = rectTmp.height;
	int R;
	if (w > h)
	{
		R = w;
	}
	else
	{
		R = h;
	}
	int iRadius;
	if (5*w < imgTmp.cols && 5*h < imgTmp.rows)
	{
		iRadius = 2*R;
	}
	else
	{
		iRadius = R;
	}

	circle(imgMask, pCenter, iRadius, 255, -1);

	return imgMask;
}
Mat matchGMS(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2, vector<DMatch>& vecInlineMatches)
{
	//Ptr<cv::ORB> orbD = cv::ORB::create(50000);
	Ptr<xfeatures2d::SURF> orbD = xfeatures2d::SURF::create(5, 4, 6, true, true);
	Ptr<cv::FastFeatureDetector> fastD = FastFeatureDetector::create(3);
	//Ptr<cv::xfeatures2d::FREAK> freakD = xfeatures2d::FREAK::create(false, false);

	cv::Mat imgDes1, imgDes2;
	Mat maskT;
	//maskT = imread("..//光州测试图片//model//4.jpg", 0);
	orbD->detect(img1, vecP1, maskT);
	orbD->detect(img2, vecP2);
	//fastD->detect(img1, vecP1);
	//fastD->detect(img2, vecP2);

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

	if (FlagIsShow)
	{
		cout << "gmsSize:" << num_inliners << endl;
		cvtColor(img1, img1, COLOR_GRAY2BGR);
		cvtColor(img2, img2, COLOR_GRAY2BGR);
		Mat show = DrawInlier(img1, img2, vecP1new, vecP2new, vecInlineMatches, 2);
		resize(show, show, Size(960, 320));
		imshow("gms", show);
		//waitKey(0);

	}
	//-----------test---------------------------------
	vector<Point> vecP1Test, vecP2Test;
	for (int i = 0; i < vecInlineMatches.size(); i++)
	{
		vecP1Test.push_back(vecP1new[vecInlineMatches[i].queryIdx].pt);
		vecP2Test.push_back(vecP2new[vecInlineMatches[i].trainIdx].pt);
	}
	vector<int> vecMask;
	Mat HTest;
	if (vecInlineMatches.size() > 4)
	{
		HTest = findHomography(vecP2Test, vecP1Test, vecMask, RANSAC);
	}
	vector<DMatch> vecMatchesTest;
	for (int i = 0; i < vecMask.size(); i++)
	{
		if (vecMask[i])
		{
			vecMatchesTest.push_back(vecInlineMatches[i]);
		}
	}

	vecMatchesTest.swap(vecInlineMatches);

	if (FlagIsShow)
	{
		Mat show1 = DrawInlier(img1, img2, vecP1new, vecP2new, vecInlineMatches, 2);
		resize(show1, show1, Size(960, 320));
		imshow("gms+ransac", show1);
		waitKey(0);
	}

	return HTest;
}

Mat matchRansac(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2)
{
	Mat hTest;
	Ptr<cv::FastFeatureDetector> fastD = FastFeatureDetector::create(10);
	Ptr<cv::xfeatures2d::FREAK> freakD = xfeatures2d::FREAK::create(false, false);

	cv::Mat imgDes1, imgDes2;
	Mat maskT;
	fastD->detect(img1, vecP1);
	fastD->detect(img2, vecP2);
	vector<KeyPoint> vecP2New;
	keyPointResize(vecP2, vecP2New, 10000);



	freakD->compute(img1, vecP1, imgDes1);
	freakD->compute(img2, vecP2New, imgDes2);


	vector<DMatch> matchesPR, matchesPRInv;
	vector<DMatch> good_matches;
	BFMatcher matcher(NORM_HAMMING, true);

	try
	{
		matcher.match(imgDes1, imgDes2, matchesPR);
		matcher.match(imgDes2, imgDes1, matchesPRInv);
		DoubleMatch(matchesPR, matchesPRInv, good_matches);
	}
	catch (const std::exception&)
	{

	}


	vector<Point2f> img, imgTmp1;
	for (int i = 0; i < good_matches.size(); i++)
	{
		img.push_back(vecP2New[good_matches[i].trainIdx].pt);
		imgTmp1.push_back(vecP1[good_matches[i].queryIdx].pt);
	}
	vector<int> vecMask;

	if (good_matches.size() > 4)
	{
		hTest = findHomography(img, imgTmp1, vecMask, RANSAC);
	}
	return hTest;
}


void test2()
{
	char* fileTemp = "..//光州测试图片//Model";
	char* fileDet = "..//光州测试图片//src";

	vector<string> filesTmp;
	vector<string> filesDet;

	getFiles(fileTemp, filesTmp);
	getFiles(fileDet, filesDet);

	vector<vector<int>> vecRect;
	fstream fr("..//光州测试图片//Model//标定信息.txt", ios::in);
	int iLable, iX1, iY1, iX2, iY2;
	while (!fr.eof())
	{
		fr >> iLable >> iX1 >> iY1 >> iX2 >> iY2;
		vector<int>temp;
		temp.push_back(iLable);
		temp.push_back(iX1);
		temp.push_back(iY1);
		temp.push_back(iX2);
		temp.push_back(iY2);
		vecRect.push_back(temp);
	}

	for (int i = 0; i < filesDet.size(); i++)
	{
		string strDet = filesDet[i];
		std::cout << strDet;

		char* h = strrchr((char *)(strDet.c_str()), '_');
		char* l = strrchr((char*)(strDet.c_str()), '\\');
		string strTempDet = strDet.substr(l - (char *)(strDet.c_str()) + 1, h - l - 1);
		for (int j = 0; j < filesTmp.size(); j++)
		{
			string strTmp = filesTmp[j];
			char* h1 = strrchr((char *)(strTmp.c_str()), '_');
			char* l1 = strrchr((char*)(strTmp.c_str()), '\\');
			string strTempTmp = strTmp.substr(l1 - (char *)(strTmp.c_str()) + 1, h1 - l1 - 1);

			if (strTempDet == strTempTmp)
			{
				Mat imgSrcTmp = imread(filesTmp[j], 0);
				Mat imgSrcDet = imread(filesDet[i], 0);
				vector<KeyPoint> vecP1, vecP2;
				vector<DMatch> vecMatch;
				
				//Point PointDst = getDiffNew(vecMatch, vecP1, vecP2, imgSrcTmp);
				//Point PointDst;
				//Servo(imgSrcTmp, imgSrcDet, 0, PointDst);

				int iTemp = atoi(strTempTmp.c_str());
				for (int k = 0; k < vecRect.size(); k++)
				{
					if (iTemp == vecRect[k][0])
					{
						Rect rectTemp;
						rectTemp.x = vecRect[k][1];
						rectTemp.y = vecRect[k][2];
						rectTemp.width = vecRect[k][3];
						rectTemp.height = vecRect[k][4];
						//Mat imgMask = getImgMask(rectTemp, imgSrcTmp);
						Mat imgMask;
						//imshow("mask", imgMask);
						//waitKey(0);
						Mat HTest = matchGMS(imgSrcTmp, imgSrcDet, vecP1, vecP2, imgMask);
						//Mat HTest = matchRansac(imgSrcTmp, imgSrcDet, vecP1, vecP2);

						Mat imgRoi;
						try
						{
							Mat temp;
							warpPerspective(imgSrcDet, temp, HTest, imgSrcDet.size());
							imgRoi = temp(rectTemp);
							//imshow("dstroi", imgRoi);
							//waitKey(0);
							std::cout << "----Y" << endl;
						}
						catch (const std::exception&)
						{
							imgRoi = imgSrcDet;
							std::cout << "------N" << endl;
						}

						char* fileDet = "..//光州测试图片//gms+warp";
						string strNameTemp = (string)fileDet + (string)l;
						imwrite(strNameTemp, imgRoi);
					}
				}


			}

		}
	}


}

void test3()
{
	FlagIsShow = true;
	cv::Mat img1, img2;

	img1 = imread("..//光州测试图片//model//04_1 (4).jpg", 0);
	img2 = imread("..//光州测试图片//src//04_1 (156).jpg", 0);
	Mat img1S, img2S;
	//resize(img1, img1S, Size(), 0.5, 0.5);
	//resize(img2, img2S, Size(), 0.5, 0.5);

	vector<cv::KeyPoint> vecP1, vecP2;
	vector<DMatch> vecInlineMatches;
	vector<DMatch> vecInlineMatches2;
	Point testDiff2;
	Servo(img1, img2, 0, testDiff2);
	cout << "DM:" << testDiff2 << endl;

	matchGMS(img1, img2, vecP1, vecP2, vecInlineMatches);
	Point testDiff = getDiffNew(vecInlineMatches, vecP1, vecP2, img1);
	cout << "GMS:" << testDiff << endl;
	Point diffOld = getDiff(vecInlineMatches, vecP1, vecP2, img1);
	cout << "old:" << diffOld << endl;

	//matchMath(img1, img2, vecP1, vecP2, vecInlineMatches2);
	//matchGMS(img1, img2, vecP1, vecP2, vecInlineMatches);
	//matchGMS(img1S, img2S, vecP1, vecP2, vecInlineMatches3);
	//Point testDiff3 = getDiff(vecInlineMatches3, vecP1, vecP2, img1S);
	//testDiff3.x = testDiff3.x * 2;
	//testDiff3.y = testDiff3.y * 2;
	//cout << "Scale:" << testDiff3 << endl;


	cout << "sizeGMS:" << vecInlineMatches.size() << endl;
	//cout << "sizeDM:" << vecInlineMatches2.size() << endl;


}

int main()
{
	//test1();
	test2();
	//test3();

	return 1;
}