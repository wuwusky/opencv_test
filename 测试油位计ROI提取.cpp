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
bool flagShow = false;

void getFiles(string path, vector<string>& files);

Point getDiff(vector<DMatch> vecInlineMatches, vector<cv::KeyPoint> vecP1, vector<cv::KeyPoint> vecP2, Mat imgS);
void matchGMS(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2, vector<DMatch>& vecInlineMatches);

Mat getBin(Mat Src)
{
	Mat imgGray;
	if (Src.channels() > 1)
	{
		cvtColor(Src, imgGray, COLOR_BGR2GRAY);
	}
	else
	{
		imgGray = Src.clone();
	}
	int iSize = Src.rows / 10;
	if (iSize % 2 == 0)
	{
		iSize++;
	}
	Mat imgBin2;
	adaptiveThreshold(imgGray, imgBin2, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, iSize, 0);

	return imgBin2;
}

Mat getEllipse(Mat imgSrc)
{
	Mat imgBin = imgSrc.clone();
	Mat imgDst;
	cvtColor(imgSrc, imgDst, COLOR_GRAY2BGR);
	Mat kern = getStructuringElement(MORPH_RECT, Size(3, 3));
	morphologyEx(imgBin, imgBin, MORPH_ERODE, kern);
	//imshow("morph", imgBin);
	//waitKey(0);


	vector<vector<Point>> contours;
	findContours(imgBin, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	vector<RotatedRect> vecEllipse;
	vector<RotatedRect> vecEllipseGood;
	for (int i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > 100)
		{
			vecEllipse.push_back(fitEllipse(contours[i]));
		}
	}
	for (int i=0;i< vecEllipse.size();i++)
	{
		if (vecEllipse[i].size.width < 200 && vecEllipse[i].size.height < 200)
		{
			if (abs(vecEllipse[i].center.x - 125) <=30 && abs(vecEllipse[i].center.y - 125) <= 30 )
			{
				vecEllipseGood.push_back(vecEllipse[i]);
			}
		}
	}
	for (int i = 0; i < vecEllipseGood.size(); i++)
	{
		ellipse(imgDst, vecEllipseGood[i], Scalar(0, 255, 0), 3);
	}

	//imshow("dst", imgDst);
	//waitKey(0);
	return imgDst;
}

Mat getEllipse(Mat imgSrc, Mat& imgMask)
{
	Mat imgBin = imgSrc.clone();
	Mat imgDst;
	cvtColor(imgSrc, imgDst, COLOR_GRAY2BGR);
	if (imgMask.rows !=0 && imgMask.cols !=0)
	{

	}
	else
	{
		//Mat kern = getStructuringElement(MORPH_CROSS, Size(5, 3));
		//morphologyEx(imgBin, imgBin, MORPH_CLOSE, kern);
		//imshow("testM", imgBin);
		//waitKey(0);
		Mat kern1 = getStructuringElement(MORPH_RECT, Size(5, 5));
		morphologyEx(imgBin, imgBin, MORPH_ERODE, kern1);
		//imshow("morph", imgBin);
		//waitKey(0);
	}
	vector<vector<Point>> contours;
	findContours(imgBin, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	//-----------test----------------------------
	Mat temp;
	for (int i=0;i< contours.size();i++)
	{
		if (contours[i].size() > 50)
		{
			Mat imgTemp = imgBin.clone();
			cvtColor(imgTemp, imgTemp, COLOR_GRAY2BGR);
			for (int j=0;j< contours[i].size();j++)
			{
				circle(imgTemp, contours[i][j], 1, Scalar(0, 0, 255), 1);

			}
			//imshow("contours", imgTemp);
			//waitKey(0);


		}
	}



	//-----------test----------------------------
	vector<RotatedRect> vecEllipse;
	vector<RotatedRect> vecEllipseGood;
	for (int i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > 100)
		{
			vecEllipse.push_back(fitEllipse(contours[i]));
		}
	}
	for (int i = 0; i < vecEllipse.size(); i++)
	{
		if (vecEllipse[i].size.width < 200 && vecEllipse[i].size.height < 200)
		{
			if (abs(vecEllipse[i].center.x - 125) <= 30 && abs(vecEllipse[i].center.y - 125) <= 30)
			{
				vecEllipseGood.push_back(vecEllipse[i]);
			}
		}
	}
	for (int i = 0; i < vecEllipseGood.size(); i++)
	{
		ellipse(imgDst, vecEllipseGood[i], Scalar(0, 255, 0), 3);
	}
	imgMask = Mat::zeros(imgBin.size(), CV_8UC1);
	for (int i=0;i< vecEllipseGood.size();i++)
	{
		ellipse(imgMask, vecEllipseGood[i], 255, -1);
	}
	//imshow("dst", imgDst);
	//imshow("mask", imgMask);
	//waitKey(0);
	return imgDst;
}

Mat getImgPrepro(Mat imgSrc)
{
	double x1 = 250.0 / (double)imgSrc.cols;
	double y1 = 250.0 / (double)imgSrc.rows;
	Mat imgSrcScale;
	resize(imgSrc, imgSrcScale, Size(), x1, y1);

	Mat imgDst;
	bilateralFilter(imgSrcScale, imgDst, 11, 150, 150);
	//imshow("test", imgDst);
	//waitKey(0);
	return imgDst;
}

Mat getImgHSV(Mat imgSrc)
{
	vector<Mat> vecHsv;
	Mat temp;
	cvtColor(imgSrc, temp, COLOR_BGR2HSV);
	split(temp, vecHsv);
	//imshow("H", vecHsv[0]);
	GaussianBlur(vecHsv[0], vecHsv[0], Size(11, 11), 5);
	//imshow("gaussH", vecHsv[0]);
	//imshow("S", vecHsv[1]);
	GaussianBlur(vecHsv[1], vecHsv[1], Size(5, 5), 15);
	//imshow("gaussS", vecHsv[1]);
	//imshow("V", vecHsv[2]);
	Mat imgTest = abs(0.1*vecHsv[0] + 0.7*vecHsv[2] + 0.2*vecHsv[1]);
	//imshow("H-V-S", imgTest);
	//imshow("Sequliaze", vecHsv[1]);
	//waitKey(0);
	Mat imgBinH, imgBinS, imgBinV;
	//threshold(vecHsv[0], imgBinH, 0, 255, THRESH_OTSU);
	//threshold(vecHsv[1], imgBinS, 0, 255, THRESH_OTSU);
	//threshold(vecHsv[2], imgBinV, 0, 255, THRESH_OTSU);
	adaptiveThreshold(vecHsv[0], imgBinH, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 51, 0);
	adaptiveThreshold(vecHsv[1], imgBinS, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 51, 0);
	adaptiveThreshold(vecHsv[2], imgBinV, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 51, 0);

	imgBinV = ~imgBinV;
	//imshow("HBin", imgBinH);
	//imshow("SBin", imgBinS);
	//imshow("VBin", imgBinV);
	Mat kernHS = getStructuringElement(MORPH_CROSS, Size(3, 17));
	Mat kernV = getStructuringElement(MORPH_RECT, Size(9, 1));
	morphologyEx(imgBinH, imgBinH, MORPH_CLOSE, kernHS);
	morphologyEx(imgBinS, imgBinS, MORPH_CLOSE, kernHS);
	morphologyEx(imgBinV, imgBinV, MORPH_CLOSE, kernV);
	imshow("HBinM", imgBinH);
	imshow("SBinM", imgBinS);
	imshow("VBinM", imgBinV);

	//waitKey(0);
	Mat imgHS, imgSV;
	imgHS = imgBinH | imgBinS;
	imshow("HS", imgHS);
	imgSV = imgBinS | imgBinV;
	imshow("SV", imgSV);
	Mat imgHV = imgBinH & imgBinV;
	imshow("HV", imgHV);
	Mat imgHSV = imgSV & imgHV;
	imshow("HSV", imgHSV);

	waitKey(0);

	return imgHSV;
}

void CopyImgFromMask(Mat& src, Mat mask)
{
	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			int aa = mask.at<uchar>(i, j);
			if (aa == 255)
			{
				src.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
			}
		}
	}

}

void test1()
{
	const char* filesPath = "..//油位计检测结果//roi//*.*";
	string filePath = "..//油位计检测结果//roi//";

	struct _finddata_t fileInfo;
	long handle = _findfirst(filesPath, &fileInfo);

	if (handle == -1L)
	{
		cout << "文件夹下无文件" << endl;
	}
	do
	{
		if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)
		{
			string fileSavePath = "..//油位计检测结果//preEllipseRoi//";
			string fileName = fileInfo.name;
			string fileCurrentPath = filePath + fileName;
			Mat imgSrc = imread(fileCurrentPath);
			Mat imgTemp = getImgPrepro(imgSrc);
			//-------test HSV------------------------------
			Mat imgTest = getImgHSV(imgTemp);
			//-------test--------------------------------
			Mat imgBin = getBin(imgTemp);
			imgBin = imgBin & imgTest;
			Mat imgMask;
			Mat imgDst = getEllipse(imgBin, imgMask);
			resize(imgMask, imgMask, imgSrc.size());
			Mat imgSrcS;
			imgSrc.copyTo(imgSrcS, imgMask);


			fileSavePath = fileSavePath + fileName;
			imwrite(fileSavePath, imgSrcS);
		}


	} while (_findnext(handle, &fileInfo) == 0);


}

Mat matchAndSave(Mat img1, Mat img2)
{
	Ptr<FastFeatureDetector> fd  = cv::FastFeatureDetector::create(10, true);
	vector<KeyPoint> vecKP1, vecKP2;
	Mat mKD1, mKD2;
	Mat img1G, img2G;
	cvtColor(img1, img1G, COLOR_BGR2GRAY);
	cvtColor(img2, img2G, COLOR_BGR2GRAY);
	fd->detect(img1G, vecKP1);
	fd->detect(img2G, vecKP2);
	Ptr<xfeatures2d::FREAK> fc = xfeatures2d::FREAK::create();
	fc->compute(img1G, vecKP1, mKD1);
	fc->compute(img2G, vecKP2, mKD2);

	vector<DMatch> vecMatch;
	//FlannBasedMatcher fbMatch;
	BFMatcher fbMatch(NORM_HAMMING, true);
	fbMatch.match(mKD1, mKD2, vecMatch);
	double distMin = 100;
	for (int i=0;i< vecMatch.size();i++)
	{
		if (distMin > vecMatch[i].distance)
		{
			distMin = vecMatch[i].distance;
		}
	}
	vector<DMatch> vecMatchGood;
	for (int i=0;i< vecMatch.size();i++)
	{
		if (vecMatch[i].distance <= distMin * 10)
		{
			vecMatchGood.push_back(vecMatch[i]);
		}
	}
	vector<Point> vecP1, vecP2;
	for (int i=0;i< vecMatchGood.size();i++)
	{
		vecP1.push_back(vecKP1[vecMatchGood[i].queryIdx].pt);
		vecP2.push_back(vecKP2[vecMatchGood[i].trainIdx].pt);
	}

	vector<uchar> vecInlineMask;
	Mat H = findHomography(vecP2, vecP1, RANSAC,3.0, vecInlineMask);
	vector<DMatch> vecMatchGoodEnd;
	for (int i=0;i< vecInlineMask.size();i++)
	{
		if (vecInlineMask[i] == 1)
		{
			vecMatchGoodEnd.push_back(vecMatchGood[i]);
		}
	}
	//Mat imgDst;
	//drawMatches(img1, vecKP1, img2, vecKP2, vecMatchGoodEnd, imgDst);
	//imshow("test", imgDst);
	//waitKey(0);
	return H;
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

void test2()
{
	char* fileTemp = "..//油位计检测结果//model";
	char* fileDet = "..//油位计检测结果//src";

	vector<string> filesTmp;
	vector<string> filesDet;

	getFiles(fileTemp, filesTmp);
	getFiles(fileDet, filesDet);

	vector<vector<int>> vecRect;
	fstream fr("标定信息.txt",ios::in);
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

	for (int i=0;i< filesDet.size();i++)
	{
		string strDet = filesDet[i];
		cout << strDet ;

		char* h = strrchr((char *)(strDet.c_str()), '_');
		char* l = strrchr((char*)(strDet.c_str()), '\\');
		string strTempDet = strDet.substr(l - (char *)(strDet.c_str()) + 1, h - l - 1);
		for (int j=0;j< filesTmp.size();j++)
		{
			string strTmp = filesTmp[j];
			char* h1 = strrchr((char *)(strTmp.c_str()), '_');
			char* l1 = strrchr((char*)(strTmp.c_str()), '\\');
			string strTempTmp = strTmp.substr(l1 - (char *)(strTmp.c_str()) + 1, h1 - l1 - 1);

			if (strTempDet == strTempTmp)
			{
				//cout << strTempDet << "--" << strTempTmp << endl;
				Mat imgSrcTmp = imread(filesTmp[j]);
				Mat imgSrcDet = imread(filesDet[i]); 
				Mat HTransform = matchAndSave(imgSrcTmp, imgSrcDet);
				int iTemp = atoi(strTempTmp.c_str());
				for (int k=0;k< vecRect.size();k++)
				{
					if (iTemp == vecRect[k][0])
					{
						Rect rectTemp;
						rectTemp.x = vecRect[k][1];
						rectTemp.y = vecRect[k][2];
						rectTemp.width = vecRect[k][3];
						rectTemp.height = vecRect[k][4];
						Mat imgDst;
						imgDst = imgSrcTmp(rectTemp);
						//imshow("ori", imgDst);
						try
						{
							warpPerspective(imgSrcDet, imgDst, HTransform, imgSrcDet.size());
							cout << "----Y" << endl;
						}
						catch (const std::exception&)
						{
							//imgDst = imgSrcDet;
							cout << "------N" << endl;
						}
						Mat imgRoi = imgDst(rectTemp);
						//imshow("roi", imgRoi);
						//waitKey(0);
						char* fileDet = "..//油位计检测结果//preRoi";
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
	string addr = "00002_0015.jpg";
	Mat imgSrc = imread("..//油位计检测结果//roi//"+addr);

	Mat imgSrcScale = getImgPrepro(imgSrc);
	Mat imgTest = getImgHSV(imgSrcScale);
	imshow("HSV", imgTest);
	Mat imgBin = getBin(imgSrcScale);
	imshow("bin", imgBin);
	waitKey(0);
	imgBin = imgTest & imgBin;
	imshow("bin&hsv", imgBin);
	Mat imgMask;
	Mat imgDst = getEllipse(imgBin, imgMask);
	imshow("dst", imgDst);
	waitKey(0);
	resize(imgMask, imgMask, imgSrc.size());
	Mat imgSrcS;
	imgSrc.copyTo(imgSrcS, imgMask);
	imshow("ROI", imgSrcS);
	waitKey(0);
	//Mat imgSrcScaleS = getImgPrepro(imgSrcS);
	//Mat imgBinS = getBin(imgSrcScaleS);
	//Mat imgDstS = getEllipse(imgBinS, imgMask);


}

void test4()
{
	string addr = "00001_0005.jpg";
	Mat imgSrc = imread("..//油位计检测结果//roi//" + addr);

	Mat imgSrcScale = getImgPrepro(imgSrc);
	Mat maskLSC;
	Mat imgDstLSC;
	Mat labelLSC;
	Ptr<ximgproc::SuperpixelLSC>  LSC;
	LSC = ximgproc::createSuperpixelLSC(imgSrcScale);
	LSC->iterate(10);
	LSC->enforceLabelConnectivity(55);
	LSC->getLabels(labelLSC);
	LSC->getLabelContourMask(maskLSC);
	int numLSC = LSC->getNumberOfSuperpixels();

	cout << "LSC:" << numLSC << endl;
	Mat temp = Mat::zeros(imgSrcScale.size(), CV_8UC3);
	CopyImgFromMask(temp, maskLSC);
	imshow("LSCMASK", temp);
	waitKey(0);
	Mat imgBin = ~(getBin(temp));
	imshow("bin", imgBin);
	waitKey(0);
	Mat imgMask;
	Mat imgDst = getEllipse(imgBin, imgMask);


}

void test5()
{
	char* fileTemp = "..//油位计检测结果//roiModel";
	char* fileDet = "..//油位计检测结果//roi";

	vector<string> filesTmp;
	vector<string> filesDet;

	getFiles(fileTemp, filesTmp);
	getFiles(fileDet, filesDet);

	vector<vector<int>> vecRect;
	fstream fr("标定信息roi.txt", ios::in);
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
		cout << strDet;

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
				//cout << strTempDet << "--" << strTempTmp << endl;
				Mat imgSrcTmp = imread(filesTmp[j], 0);
				Mat imgSrcDet = imread(filesDet[i], 0);
				vector<KeyPoint> vecP1, vecP2;
				vector<DMatch> vecMatch;
				//matchGMS(imgSrcTmp, imgSrcDet, vecP1, vecP2, vecMatch);
				//Point PointDst = getDiff(vecMatch, vecP1, vecP2, imgSrcTmp);
				Point PointDst;
				Servo(imgSrcTmp, imgSrcDet, 0, PointDst);

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
						Mat imgDst;
						imgDst = imgSrcTmp(rectTemp);
						//imshow("srcori", imgDst);
						//waitKey(0);
						rectTemp.x = rectTemp.x - PointDst.x;
						rectTemp.y = rectTemp.y - PointDst.y;
						Mat imgRoi;
						try
						{
							imgRoi = imgSrcDet(rectTemp);
							//imshow("dstroi", imgRoi);
							//waitKey(0);
							cout << "----Y" << endl;
						}
						catch (const std::exception&)
						{
							//imgDst = imgSrcDet;
							cout << "------N" << endl;
						}

						char* fileDet = "..//油位计检测结果//preRoi3";
						string strNameTemp = (string)fileDet + (string)l;
						imwrite(strNameTemp, imgRoi);
					}
				}


			}

		}
	}


}



int main()
{

	//test1();

	//test2();

	//test3();

	//test4();

	test5();
	return 1;
}

Point getDiff(vector<DMatch> vecInlineMatches, vector<cv::KeyPoint> vecP1, vector<cv::KeyPoint> vecP2, Mat imgS)
{
	vector<int> vecDx(imgS.cols * 2+2);
	vector<int> vecDy(imgS.rows * 2+2);
	for (int i=0;i< vecDx.size();i++)
	{
		vecDx[i] = 0;
	}
	for (int i=0;i< vecDy.size();i++)
	{
		vecDy[i] = 0;
	}

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

void matchGMS(Mat img1, Mat img2, vector<KeyPoint>& vecP1, vector<KeyPoint>& vecP2, vector<DMatch>& vecInlineMatches)
{
	//Ptr<cv::ORB> orbD = cv::ORB::create(10000);
	Ptr<xfeatures2d::SURF> orbD = xfeatures2d::SURF::create(5, 4, 6, true, true);

	cv::Mat imgDes1, imgDes2;

	orbD->detectAndCompute(img1, Mat(), vecP1, imgDes1);
	orbD->detectAndCompute(img2, Mat(), vecP2, imgDes2);
	if (vecP2.size() < 4)
	{
		vecP2 = vecP1;
		imgDes2 = imgDes1;
	}

	vector<DMatch> vecMatches;

	//BFMatcher match1(NORM_L2);
	FlannBasedMatcher match1;

	match1.match(imgDes1, imgDes2, vecMatches);
	//match1.match(imgDes2, imgDes1, vecMatches2);


	//Mat imgDst;
	//drawMatches(img1, vecP1, img2, vecP2, vecMatches, imgDst, Scalar(0, 255, 255), Scalar(0, 255, 255));
	//resize(imgDst, imgDst, Size(960, 320));
	////imshow("match1", imgDst);
	//waitKey(0);

	vector<bool> vecInliners;
	gms_matcher gms(vecP1, img1.size(), vecP2, img2.size(), vecMatches);
	int num_inliners = gms.GetInlierMask(vecInliners, true, false);
	for (int i = 0; i < vecInliners.size(); i++)
	{

		if (vecInliners[i])
		{
			vecInlineMatches.push_back(vecMatches[i]);
		}
	}

	cvtColor(img1, img1, COLOR_GRAY2BGR);
	cvtColor(img2, img2, COLOR_GRAY2BGR);
	//Mat show = DrawInlier(img1, img2, vecP1, vecP2, vecInlineMatches, 2);
	//resize(show, show, Size(960, 320));
	//imshow("gms", show);



}
