#include <opencv2/opencv.hpp>
#include <iostream>
#include <io.h>
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
using namespace std;
using namespace cv;
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
Mat getImgPre(Mat imgSrc);
void getFiles(string path, vector<string>&files, vector<string>& fileNames);


Mat getImgPre(Mat imgSrc)
{
	Mat imgHSV;
	Mat imgDst;
	cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);
	vector<Mat> hsvChannels;
	split(imgHSV, hsvChannels);

	//imshow("S", hsvChannels[1]);
	//waitKey(0);
	Mat imgMask;
	threshold(hsvChannels[1], imgMask, 0, 255, THRESH_OTSU);
	//imshow("Mask", imgMask);
	//waitKey(0);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(imgMask, imgMask, MORPH_DILATE, kernel, Point(-1, -1), 2);
	//imshow("morph", imgMask);
	//waitKey(0);
	//Mat imgMask2 = Mat::zeros(imgMask.size(), CV_8UC1);
	//rectangle(imgMask2, Point(0, imgMask2.rows / 4), Point(imgMask2.cols - 1, (imgMask2.rows / 4) * 3), 255, -1);
	//imshow("Mask2", imgMask2);
	//imgMask = imgMask & imgMask2;
	//imshow("mask", imgMask);
	Mat imgRoi;
	Mat imgBi;
	bilateralFilter(imgSrc, imgBi, 5, 50, 150);

	imgBi.copyTo(imgRoi, imgMask);
	//imshow("Roi", imgRoi);
	//waitKey(0);

	Mat imgRoiGray;
	cvtColor(imgRoi, imgRoiGray, COLOR_BGR2GRAY);
	Mat imgX, imgY;
	Sobel(imgRoiGray, imgX, imgRoi.depth(), 0, 1, 3);
	//imshow("sobelX", imgX);
	Sobel(imgRoiGray, imgY, imgRoi.depth(), 1, 0, 3);
	//imshow("sobelY", imgY);
	imgDst = imgX + imgY;
	//imshow("X+Y", imgDst);
	//waitKey(0);
	return imgDst;
}

Mat getImgPre2(Mat imgSrc)
{
	Mat imgHSV;
	Mat imgDst;
	cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);
	vector<Mat> hsvChannels;
	split(imgHSV, hsvChannels);

	//imshow("S", hsvChannels[1]);
	//waitKey(0);
	Mat imgMask;
	threshold(hsvChannels[1], imgMask, 0, 255, THRESH_OTSU);
	//imshow("Mask", imgMask);
	//waitKey(0);
	Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(imgMask, imgMask, MORPH_CLOSE, kernel, Point(-1, -1), 2);
	morphologyEx(imgMask, imgMask, MORPH_OPEN, kernel, Point(-1, -1), 2);
	//imshow("morph", imgMask);
	Mat imgBi;
	bilateralFilter(imgSrc, imgBi, 5, 150, 150);
	vector<vector<Point>> contours;
	findContours(imgMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	int iSizeMax = 0;
	for (int i=0;i< contours.size();i++)
	{
		if (iSizeMax < contours[i].size())
		{
			iSizeMax = contours[i].size();
		}
	}
	vector<vector<Point>> contoursGood;
	for (int i=0;i< contours.size();i++)
	{
		if (contours[i].size() > iSizeMax/3)
		{
			contoursGood.push_back(contours[i]);
		}
	}
	
	vector<Rect> vecRoiRect;
	for (int i=0;i< contoursGood.size();i++)
	{
		Rect rectTemp;
		rectTemp = boundingRect(contoursGood[i]);
		vecRoiRect.push_back(rectTemp);
	}
	imgDst = imgBi.clone();
	for (int i=0;i< vecRoiRect.size();i++)
	{
		rectangle(imgDst, vecRoiRect[i], Scalar(0, 255, 0), 2);
	}
	return imgDst;
}

Mat getImgPre3(Mat imgSrc)
{
	Mat imgGray;
	cvtColor(imgSrc, imgGray, COLOR_BGR2GRAY);
	Mat imgDst;
	equalizeHist(imgGray, imgDst);
	return imgDst;
}

Mat getImgPre4(Mat imgSrc)
{
	Mat imgHSV;
	Mat imgDst;
	cvtColor(imgSrc, imgHSV, COLOR_BGR2HSV);
	vector<Mat> hsvChannels;
	split(imgHSV, hsvChannels);
	//equalizeHist(hsvChannels[1], imgDst);
	imgDst = hsvChannels[1].clone();
	return imgDst;

}

Mat getImgPre5(Mat imgSrc)
{
	Mat imgDst;
	vector<Mat> vecChanels;
	split(imgSrc, vecChanels);
	for (int i=0;i< vecChanels.size();i++)
	{
		equalizeHist(vecChanels[i], vecChanels[i]);
		Mat temp = vecChanels[i].clone();
		bilateralFilter(temp, vecChanels[i], 15, 100, 100);
	}
	merge(vecChanels, imgDst);
	return imgDst;
}

void main()
{
	//Mat imgSrc,imgDst;
	//imgSrc = imread("..//压板//红合.jpg", 1);
	//Mat imgGray;
	//cvtColor(imgSrc, imgGray, COLOR_BGR2GRAY);
	//imshow("src", imgSrc);
	//imshow("gray", imgGray);
	//Mat imgEn;
	//equalizeHist(imgGray, imgEn);
	//imshow("equalize", imgEn);
	//Mat imgSrc2 = imread("..//压板//红合.jpg", 1);
	////Mat imgDst2 = getImgPre(imgSrc2);
	//Mat imgDst2 = getImgPre5(imgSrc2);
	//imshow("分", imgDst2);
	//imgDst = getImgPre2(imgSrc);
	//imshow("合", imgDst);

	//waitKey(0);


	string addr = "..\\yabanTrain";
	string addrS = "..//yanbanTestRGB//";
	vector<string>vecFiles, vecFileNames;
	getFiles(addr, vecFiles, vecFileNames);
	for (int i=0;i< vecFiles.size();i++)
	{
		cout << i + 1 << endl;
		Mat imgSrc = imread(vecFiles[i]);
		Mat imgDst = getImgPre5(imgSrc);
		imwrite(addrS + vecFileNames[i], imgDst);
	}

}

void getFiles(string path, vector<string>&files, vector<string>& fileNames)
{
	long hFile = 0;
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
