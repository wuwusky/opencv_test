#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/ximgproc/edge_filter.hpp>
#include "inpaint/inpaint.h"
#include <opencv2/xphoto/inpainting.hpp>
#include <io.h>
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
using namespace std;
using namespace cv;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define Light 1
#define NetInside 2
#define Net 3

void myTestImgPrePro(cv::Mat& img, cv::Mat& imgLabel, cv::Mat& imgBinMask);
void myTestFix(cv::Mat& imgSrc, cv::Mat& imgDst, cv::Mat& imgLabel);
bool nbIsNetInside(cv::Mat mLable, int i, int j);
bool nbIsLight(cv::Mat mLable, int i, int j, vector<cv::Vec2i>& vecLightPos);
bool getNbValue(cv::Mat mLable, int i, int j, cv::Mat mSrc, cv::Vec3b& dstValue);
void myTestPointer(cv::Mat& src, cv::Mat& dst, bool isTest);
bool getFiles(string path, vector<string>& vecFiles);

//----------------------------------------------------------------------------
void test1()
{
	Inpaint testInpaint;
	cv::Mat imgSrc, imgDst, imgTemp, imgGray, imgBin1, imgLable;
	imgSrc = imread("IMG_5062.jpg");
	imshow("原图", imgSrc);
	waitKey(0);
	testInpaint.init(imgSrc);
	cv::cvtColor(imgSrc, imgGray, COLOR_BGR2GRAY);
	imshow("灰度", imgGray);
	waitKey(0);
	int grayValueMean = 0;
	for (int i = 0; i < imgGray.rows; i++)
	{
		for (int j = 0; j < imgGray.cols; j++)
		{
			grayValueMean = grayValueMean + imgGray.at<uchar>(i, j);
		}
	}
	grayValueMean = grayValueMean / (imgGray.rows*imgGray.cols);

	threshold(imgGray, imgBin1, 0, 255, THRESH_OTSU);
	imshow("二值", imgBin1);
	waitKey(0);
	cv::Mat imgBin2 = imgBin1.clone();
	cv::Mat morphMat = getStructuringElement(MORPH_RECT, Size(3, 3));
	//morphologyEx(imgBin1, imgBin2, MORPH_ERODE, morphMat);
	imshow("二值腐蚀", imgBin2);
	waitKey(0);
	imgLable.create(imgSrc.size(), CV_8UC1);
	for (int i = 0; i < imgSrc.rows; i++)
	{
		for (int j = 0; j < imgSrc.cols; j++)
		{
			if (imgSrc.at<cv::Vec3b>(i, j)[2] > imgSrc.at<cv::Vec3b>(i, j)[0] + 20 && imgSrc.at<cv::Vec3b>(i, j)[2] > imgSrc.at<cv::Vec3b>(i, j)[1] + 20)
			{
				imgBin2.at<uchar>(i, j) = 255;
			}
			if (imgBin2.at<uchar>(i, j) > 0)
			{
				imgLable.at<uchar>(i, j) = Light;
			}
			else
			{
				imgLable.at<uchar>(i, j) = NetInside;
			}
		}
	}
	imshow("找红色源", imgBin2);
	waitKey(0);

	for (int i = 0; i < imgBin1.rows; i++)
	{
		for (int j = 0; j < imgBin1.cols; j++)
		{
			if (imgBin2.at<uchar>(i, j) > 0)
			{
				imgGray.at<uchar>(i, j) = grayValueMean - 50;
			}
		}
	}

	imshow("第一次填充", imgGray);
	imwrite("填充.bmp", imgGray);
	waitKey(0);

	cv::Mat imgBin3;
	threshold(imgGray, imgBin3, 80, 255, THRESH_BINARY);
	imshow("第二次二值", imgBin3);
	waitKey(0);

	//inpaint(imgSrc, imgBin3, imgDst, 33, INPAINT_TELEA);
	//imshow("尝试修复1", imgDst);
	//waitKey(0);
	for (int i = 0; i < imgBin3.rows; i++)
	{
		for (int j = 0; j < imgBin3.cols; j++)
		{
			if (imgBin3.at<uchar>(i, j) > 0)
			{
				testInpaint.mask.at<uchar>(i, j) = 2;
			}
		}
	}
	testInpaint.Process();
	imshow("尝试修复2", testInpaint.des);
	waitKey(0);

}
//----------------------------------------------------------------------------
void test2()
{
	cv::Mat imgSrc, imgDst, imgLabel, imgLabelTemp, imgMorph, imgBinMask;
	imgSrc = imread("IMG_5067.jpg");
	resize(imgSrc, imgSrc, Size(), 0.5, 0.5);
	imshow("原图", imgSrc);
	waitKey(0);

	cv::Mat morphMat = getStructuringElement(MORPH_CROSS, Size(3, 3));
	morphologyEx(imgSrc, imgMorph, MORPH_CLOSE, morphMat, Point(-1, -1), 4);
	imshow("尝试形态学1", imgMorph);
	waitKey(0);

	myTestImgPrePro(imgSrc, imgLabel, imgBinMask);
	myTestFix(imgSrc, imgDst, imgLabel);
	imshow("尝试修复1", imgDst);
	waitKey(0);


	//cv::Mat imgDstTemp, imgDstBin, imgEdge;
	//cvtColor(imgDst, imgDstTemp, COLOR_BGR2GRAY);
	//threshold(imgDstTemp, imgDstBin, 0, 255, THRESH_OTSU);
	//morphologyEx(imgDstBin, imgDstBin, MORPH_CLOSE, morphMat);
	//Canny(imgDstBin, imgEdge, 50, 200, 3);
	//imshow("修复后边缘", imgEdge);
	//waitKey(0);
	//cv::ximgproc::guidedFilter(imgEdge, imgDst, imgDst, 5, 1);
	//imshow("导向滤波", imgDst);
	//waitKey(0);

	morphologyEx(imgDst, imgDst, MORPH_CLOSE, morphMat, Point(-1, -1), 2);
	imshow("尝试形态学2", imgDst);
	waitKey(0);

	//imgLabelTemp = imgLabel.clone();
	//myTestFix(imgDst, imgDst, imgLabel);
	//imshow("尝试修复2", imgDst);
	//waitKey(0);
	//imgLabelTemp = imgLabel.clone();
	//myTestFix(imgDst, imgDst, imgLabel);
	//imshow("尝试修复3", imgDst);
	//waitKey(0);

	//Inpaint testInpaint;
	//testInpaint.init(imgSrc);
	//for (int i = 0; i < imgSrc.rows; i++)
	//{
	//	for (int j = 0; j < imgSrc.cols; j++)
	//	{
	//		if (imgBin1.at<uchar>(i, j) > 0)
	//		{
	//			testInpaint.mask.at<uchar>(i, j) = 2;
	//		}
	//		else
	//		{
	//			testInpaint.mask.at<uchar>(i, j) = 1;
	//		}
	//	}
	//}
	//testInpaint.Process();
	//imshow("尝试修复", testInpaint.des);
	//waitKey(0);

	//inpaint(imgSrc, imgBin1, imgDst, 5, INPAINT_TELEA);
	//imshow("尝试修复1", imgDst);
	//waitKey(0);


}
//----------------------------------------------------------------------------
void test3()
{
	Inpaint testInpaint;
	cv::Mat imgSrc, imgDst, imgTemp, imgGray, imgBin1, imgLable;
	imgSrc = imread("IMG_5062.jpg");
	imshow("原图", imgSrc);
	waitKey(0);
	testInpaint.init(imgSrc);
	cv::cvtColor(imgSrc, imgGray, COLOR_BGR2GRAY);
	imshow("灰度", imgGray);
	waitKey(0);
	int grayValueMean = 0;
	for (int i = 0; i < imgGray.rows; i++)
	{
		for (int j = 0; j < imgGray.cols; j++)
		{
			grayValueMean = grayValueMean + imgGray.at<uchar>(i, j);
		}
	}
	grayValueMean = grayValueMean / (imgGray.rows*imgGray.cols);

	threshold(imgGray, imgBin1, 0, 255, THRESH_OTSU);
	imshow("二值", imgBin1);
	waitKey(0);
	cv::Mat imgBin2 = imgBin1.clone();
	cv::Mat morphMat = getStructuringElement(MORPH_CROSS, Size(3, 3));
	morphologyEx(imgBin1, imgBin2, MORPH_ERODE, morphMat);
	imshow("二值腐蚀", imgBin2);
	waitKey(0);
	imgLable.create(imgSrc.size(), CV_8UC1);
	for (int i = 0; i < imgSrc.rows; i++)
	{
		for (int j = 0; j < imgSrc.cols; j++)
		{
			if (imgSrc.at<cv::Vec3b>(i, j)[2] > imgSrc.at<cv::Vec3b>(i, j)[0] + 20 && imgSrc.at<cv::Vec3b>(i, j)[2] > imgSrc.at<cv::Vec3b>(i, j)[1] + 20)
			{
				imgBin2.at<uchar>(i, j) = 255;
			}
			if (imgBin2.at<uchar>(i, j) > 0)
			{
				imgLable.at<uchar>(i, j) = Light;
			}
			else
			{
				imgLable.at<uchar>(i, j) = NetInside;
			}
		}
	}
	imshow("加红源", imgBin2);
	waitKey(0);

	for (int i = 0; i < imgBin1.rows; i++)
	{
		for (int j = 0; j < imgBin1.cols; j++)
		{
			if (imgBin2.at<uchar>(i, j) > 0)
			{
				imgGray.at<uchar>(i, j) = grayValueMean - 50;
			}
		}
	}

	imshow("第一次填充", imgGray);
	imwrite("填充.bmp", imgGray);
	waitKey(0);

	cv::Mat imgBin3;
	threshold(imgGray, imgBin3, 80, 255, THRESH_BINARY);
	imshow("第二次二值", imgBin3);
	waitKey(0);

	for (int i = 0; i < imgBin3.rows;i++)
	{
		for (int j = 0; j < imgBin3.cols; j++)
		{
			if (imgBin3.at<uchar>(i, j) > 0)
			{
				imgLable.at<uchar>(i, j) = Net;
			}
		}
	}
	
	myTestFix(imgSrc, imgDst, imgLable);




	imshow("尝试修复", imgDst);
	waitKey(0);

}

bool nbIsLight(cv::Mat mLable, int i, int j, vector<cv::Vec2i>& vecLightPos)
{
	int numLight = 0;
	if (mLable.at<uchar>(i - 1, j - 1) == Light || mLable.at<uchar>(i - 1, j) == Light || mLable.at<uchar>(i - 1, j + 1) == Light ||
		mLable.at<uchar>(i, j - 1) == Light || mLable.at<uchar>(i, j + 1) == Light ||
		mLable.at<uchar>(i + 1, j - 1) == Light || mLable.at<uchar>(i + 1, j) == Light || mLable.at<uchar>(i + 1, j + 1) == Light)
	{
		if (mLable.at<uchar>(i - 1, j - 1) == Light)
		{
			vecLightPos.push_back(Vec2i(i - 1, j - 1));
			numLight++;
		}
		if (mLable.at<uchar>(i - 1, j) == Light)
		{
			vecLightPos.push_back(Vec2i(i - 1, j ));
			numLight++;

		}
		if (mLable.at<uchar>(i - 1, j + 1) == Light)
		{
			vecLightPos.push_back(Vec2i(i - 1, j + 1));
			numLight++;

		}
		if (mLable.at<uchar>(i, j - 1) == Light)
		{
			vecLightPos.push_back(Vec2i(i, j - 1));
			numLight++;

		}
		if (mLable.at<uchar>(i, j + 1) == Light)
		{
			vecLightPos.push_back(Vec2i(i, j + 1));
			numLight++;

		}
		if (mLable.at<uchar>(i + 1, j - 1) == Light)
		{
			vecLightPos.push_back(Vec2i(i + 1, j - 1));
			numLight++;

		}
		if (mLable.at<uchar>(i + 1, j) == Light)
		{
			vecLightPos.push_back(Vec2i(i + 1, j));
			numLight++;

		}
		if (mLable.at<uchar>(i + 1, j + 1) == Light)
		{
			vecLightPos.push_back(Vec2i(i + 1, j + 1));
			numLight++;

		}
		if (numLight >= 3)
		{
			return true;
		}
		else
		{
			return false;
		}
		return true;

	}
	else
	{
		return false;
	}
}

bool nbIsNetInside(cv::Mat mLable, int i, int j)
{
	if (mLable.at<uchar>(i - 1, j - 1) == NetInside || mLable.at<uchar>(i - 1, j) == NetInside || mLable.at<uchar>(i - 1, j + 1) == NetInside ||
		mLable.at<uchar>(i, j - 1) == NetInside || mLable.at<uchar>(i, j + 1) == NetInside ||
		mLable.at<uchar>(i + 1, j - 1) == NetInside || mLable.at<uchar>(i + 1, j) == NetInside || mLable.at<uchar>(i + 1, j + 1) == NetInside)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool getNbValue(cv::Mat mLable, int i, int j, cv::Mat mSrc, cv::Vec3b& dstValue)
{
	cv::Vec3i Nbvalue = cv::Vec3i(255, 255, 255);
	cv::Vec3i valueTemp;
	int NbNum = 0;
	if (mLable.at<uchar>(i - 1, j - 1) == NetInside)
	{
		valueTemp = cv::Vec3i(mSrc.at<cv::Vec3b>(i - 1, j - 1));
		if (Nbvalue[0] > valueTemp[0] && Nbvalue[1] > valueTemp[1] && Nbvalue[2] > valueTemp[2])
		{
			Nbvalue = valueTemp;
		}
	}
	if (mLable.at<uchar>(i-1, j) == NetInside)
	{
		valueTemp = cv::Vec3i(mSrc.at<cv::Vec3b>(i - 1, j));
		if (Nbvalue[0] > valueTemp[0] && Nbvalue[1] > valueTemp[1] && Nbvalue[2] > valueTemp[2])
		{
			Nbvalue = valueTemp;
		}
	}
	if (mLable.at<uchar>(i - 1, j + 1) == NetInside)
	{
		valueTemp = cv::Vec3i(mSrc.at<cv::Vec3b>(i - 1, j + 1));
		if (Nbvalue[0] > valueTemp[0] && Nbvalue[1] > valueTemp[1] && Nbvalue[2] > valueTemp[2])
		{
			Nbvalue = valueTemp;
		}
	}
	if (mLable.at<uchar>(i, j-1) == NetInside)
	{
		valueTemp = cv::Vec3i(mSrc.at<cv::Vec3b>(i, j-1));
		if (Nbvalue[0] > valueTemp[0] && Nbvalue[1] > valueTemp[1] && Nbvalue[2] > valueTemp[2])
		{
			Nbvalue = valueTemp;
		}
	}
	if (mLable.at<uchar>(i, j + 1) == NetInside)
	{
		valueTemp = cv::Vec3i(mSrc.at<cv::Vec3b>(i, j + 1));
		if (Nbvalue[0] > valueTemp[0] && Nbvalue[1] > valueTemp[1] && Nbvalue[2] > valueTemp[2])
		{
			Nbvalue = valueTemp;
		}
	}
	if (mLable.at<uchar>(i + 1, j - 1) == NetInside)
	{
		valueTemp = cv::Vec3i(mSrc.at<cv::Vec3b>(i + 1, j - 1));
		if (Nbvalue[0] > valueTemp[0] && Nbvalue[1] > valueTemp[1] && Nbvalue[2] > valueTemp[2])
		{
			Nbvalue = valueTemp;
		}
	}
	if (mLable.at<uchar>(i + 1, j) == NetInside)
	{
		valueTemp = cv::Vec3i(mSrc.at<cv::Vec3b>(i + 1, j));
		if (Nbvalue[0] > valueTemp[0] && Nbvalue[1] > valueTemp[1] && Nbvalue[2] > valueTemp[2])
		{
			Nbvalue = valueTemp;
		}
	}
	if (mLable.at<uchar>(i + 1, j + 1) == NetInside)
	{
		valueTemp = cv::Vec3i(mSrc.at<cv::Vec3b>(i + 1, j + 1));
		if (Nbvalue[0] > valueTemp[0] && Nbvalue[1] > valueTemp[1] && Nbvalue[2] > valueTemp[2])
		{
			Nbvalue = valueTemp;
		}
	}
	dstValue = Nbvalue;
	return true;
}

void myTestFix(cv::Mat& imgSrc, cv::Mat& imgDst, cv::Mat& imgLabel)
{
	imgDst = imgSrc.clone();
	for (int i = 1; i < imgSrc.rows - 1; i++)
	{
		for (int j = 1; j < imgSrc.cols - 1; j++)
		{
			if (imgLabel.at<uchar>(i, j) == Net)
			{
				vector<Vec2i> vecPosition;
				if (nbIsLight(imgLabel, i, j, vecPosition))
				{
					cv::Vec3i dstValue = cv::Vec3i(0, 0, 0);
					cv::Vec3i dstTemp;
					for (int n = 0; n < vecPosition.size(); n++)
					{
						dstTemp = cv::Vec3i(imgSrc.at<cv::Vec3b>(vecPosition[n][0], vecPosition[n][1]));
						if (dstValue[0] < dstTemp[0] && dstValue[1] < dstTemp[1] && dstValue[2] < dstTemp[2])
						{
							dstValue = dstTemp;
						}
					}
					dstValue = dstValue;
					imgDst.at<cv::Vec3b>(i, j) = dstValue;
					imgLabel.at<uchar>(i, j) = Light;
				}
				else if (nbIsNetInside(imgLabel, i, j))
				{
					cv::Vec3b dstValue;
					getNbValue(imgLabel, i, j, imgSrc, dstValue);
					imgDst.at<cv::Vec3b>(i, j) = dstValue;
					imgLabel.at<uchar>(i, j) = NetInside;//这里得更新一下！不然后面处理麻烦。
				}


			}
		}
	}
}

void myTestImgPrePro(cv::Mat &img, cv::Mat& imgLabel, cv::Mat& imgBinMask)
{
	cv::Mat imgSrc, imgGray, imgBin1, imgTemp;
	imgSrc = img.clone();
	//imshow("修复前", imgSrc);
	//waitKey(0);
	vector<Mat> channels;
	split(imgSrc, channels);
	//imshow("B", channels[0]);
	//imshow("G", channels[1]);
	//imshow("R", channels[2]);
	//waitKey(0);
	cv::Mat imgMask;
	threshold(channels[1], imgMask, 0, 255, THRESH_OTSU);
	//imshow("灯掩膜1", imgMask);
	//waitKey(0);
	cv::Mat morphMat = getStructuringElement(MORPH_CROSS, Size(3, 3));
	morphologyEx(imgMask, imgMask, MORPH_OPEN, morphMat);
	//imshow("灯掩膜2", imgMask);
	//waitKey(0);
	imgTemp = 255 - channels[1];
	//imshow("反相", imgTemp);
	//waitKey(0);
	cv::equalizeHist(imgTemp, imgTemp);
	//imshow("均衡化", imgTemp);
	//waitKey(0);
	imgTemp = imgTemp + imgMask;
	//imshow("填充", imgTemp);
	//waitKey(0);
	adaptiveThreshold(imgTemp, imgBin1, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 35, 0);
	//imshow("最终掩膜", imgBin1);
	//waitKey(0);
	imgBinMask = imgMask.clone();

	imgLabel.create(imgSrc.size(), CV_8UC1);
	for (int i = 0; i < imgBin1.rows; i++)
	{
		for (int j = 0; j < imgBin1.cols; j++)
		{
			if (imgBin1.at<uchar>(i, j) > 0)
			{
				imgLabel.at<uchar>(i, j) = Net;
			}
			else
			{
				imgLabel.at<uchar>(i, j) = NetInside;
			}
			if (imgMask.at<uchar>(i, j) > 0)
			{
				imgLabel.at<uchar>(i, j) = Light;
			}
		}

	}

}

void myTestPointer(cv::Mat& src, cv::Mat& dst, bool isTest)
{
	cv::Mat imgSrc, imgDst, imgBin, imgTemp, imgGray;
	imgSrc = src;
	//////resize(imgSrc, imgSrc, Size(), 0.5, 0.5);

	cvtColor(imgSrc, imgGray, COLOR_BGR2GRAY);
	int iBlockSize;
	if (imgGray.rows > 600)
	{
		iBlockSize = imgGray.rows / 3;
	}
	else
	{
		iBlockSize = imgGray.rows / 5;
	}
	if (iBlockSize % 2 == 0)
	{
		iBlockSize++;
	}
	adaptiveThreshold(imgGray, imgBin, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, iBlockSize, 35);

	if (isTest)
	{
		imshow("二值", imgBin);
		waitKey(0);
	}

	cv::Mat morphMat = getStructuringElement(MORPH_CROSS, Size(5, 5));
	morphologyEx(imgBin, imgBin, MORPH_DILATE, morphMat);
	morphologyEx(imgBin, imgBin, MORPH_OPEN, morphMat);
	if (isTest)
	{
		imshow("去噪", imgBin);
		waitKey(0);
	}




	Mat temptemptemp = imgBin.clone();
	cvtColor(temptemptemp, temptemptemp, COLOR_GRAY2BGR);

	vector<vector<Point>> contours;
	imgTemp = imgBin.clone();
	findContours(imgTemp, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
	double dLengthMax = 0;
	int iLabel;
	for (int i = 0; i < contours.size(); i++)
	{
		if (contours[i].size() > 10)
		{
			RotatedRect RectTemp;
			RectTemp = fitEllipse(contours[i]);
			double dLengthTemp = arcLength(contours[i], true);
			if (RectTemp.center.x > imgBin.cols / 4 && RectTemp.center.x < imgBin.cols / 4 * 3 && RectTemp.center.y > imgBin.rows / 4 && RectTemp.center.y < imgBin.rows / 4 * 3)
			{
				if (RectTemp.size.height / RectTemp.size.width > 2 || RectTemp.size.width / RectTemp.size.height > 2)
				{
					if (dLengthTemp > dLengthMax)
					{
						iLabel = i;
						dLengthMax = dLengthTemp;


					}
				}
			}

			//ellipse(temptemptemp, RectTemp, Scalar(0, 255, 0), 2);
			//imshow("测试下", temptemptemp);
			//waitKey(0);

		}
	}
	cv::Mat temp;
	temp = cv::Mat::zeros(imgSrc.size(), CV_8UC1);
	drawContours(temp, contours, iLabel, 255, -1);
	if (isTest)
	{
		imshow("疑似指针", temp);
		waitKey(0);
	}


	cv::Mat morphMTemp = getStructuringElement(MORPH_CROSS, Size(3, 3));
	morphologyEx(temp, temp, MORPH_DILATE, morphMTemp);
	//imshow("膨胀下", temp);
	//waitKey(0);
	inpaint(imgSrc, temp, imgDst, 5, 0);
	dst = imgDst.clone();
	////imshow("修复", imgDst);
	////waitKey(0);

	////Inpaint test;
	////test.init(imgSrc, temp);
	////test.Process();
	////imshow("修复2", test.des);
	////waitKey(0);

}

void test4()
{
	bool isTest = true;


	//string strAddPath = "..\\testInpaint";
	//vector<string> vecFiles;

	//getFiles(strAddPath, vecFiles);

	//for (int i = 0; i < vecFiles.size(); i++)
	//{
	//	cv::Mat imgSrc, imgDst;
	//	imgSrc = imread(strAddPath + "\\"+ vecFiles[i]);
	//	myTestPointer(imgSrc, imgDst, isTest);
	//	string strAddSavePath = strAddPath + "\\dst\\" + vecFiles[i];
	//	imwrite(strAddSavePath, imgDst);
	//	cout << vecFiles[i] << "---完成" << endl;
	//}

	cv::Mat imgSrc, imgDst, imgBin, imgTemp, imgGray;
	imgSrc = imread("..\\testInpaint\\176.jpg");
	imshow("原图", imgSrc);
	waitKey(0);
	myTestPointer(imgSrc, imgDst, isTest);
	imshow("修复", imgDst);
	waitKey(0);


}

bool getFiles(string path, vector<string>& vecFiles)
{
	string filesPath = path + "\\" + "*.jpg*";
	const char* charFilesPath = filesPath.c_str();
	struct _finddata_t fileInfo;
	long handle = _findfirst(charFilesPath, &fileInfo);
	
	if (handle == -1L)
	{
		return false;
	}

	do
	{
		if (strcmp(fileInfo.name, ".") != 0 && strcmp(fileInfo.name, "..") != 0)
		{
			string fileName = fileInfo.name;
			vecFiles.push_back(fileName);
		}
	} while (_findnext(handle, &fileInfo) == 0);

	_findclose(handle);
}

int main()
{
	test4();
}

