#include "opencv2/opencv.hpp"
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
using namespace std;
using namespace cv;
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
Mat getImgWithoutBorder(Mat imgSrc)
{
	Mat imgDst, imgG, imgB;

	cvtColor(imgSrc, imgG, COLOR_BGR2GRAY);
	resize(imgG, imgG, Size(), 0.1, 0.1);
	threshold(imgG, imgB, 5, 255, THRESH_BINARY);

	imshow("test", imgB);
	waitKey(0);
	Mat temp = imgB.clone();
	vector<vector<Point>> vecPs;
	cv::findContours(temp, vecPs, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	int iMaxLabel;
	int iMaxSize = 0;
	for (int i = 0; i < vecPs.size(); i++)
	{
		if (iMaxSize < vecPs[i].size())
		{
			iMaxSize = vecPs[i].size();
			iMaxLabel = i;
		}
	}

	Rect recRoi;
	recRoi = cv::boundingRect(vecPs[iMaxLabel]);
	recRoi.x = recRoi.x * 10;
	recRoi.y = recRoi.y * 10;
	recRoi.width = recRoi.width * 10;
	recRoi.height = recRoi.height * 10;
	imgDst = imgSrc(recRoi).clone();

	cvtColor(imgDst, temp, COLOR_BGR2GRAY);
	imgG = temp.clone();
	resize(imgG, imgG, Size(), 0.1, 0.1);
	threshold(imgG, imgB, 5, 255, THRESH_BINARY_INV);
	Mat mK = getStructuringElement(MORPH_RECT, Size(1, 3));
	morphologyEx(imgB, imgB, MORPH_OPEN, mK);
	imshow("test2", imgB);
	waitKey(0);
	int iXL, iXR;
	iXL = iXR = 0;

	for (int j=0;j< imgB.cols/3;j++)
	{
		int iNumB = 0;

		for (int i=0;i< imgB.rows;i++)
		{
			if (imgB.at<uchar>(i, j) < 200)
			{
				iNumB++;
			}
		}
		if (iNumB == imgB.rows)
		{
			iXL = j;
			break;
		}
	}
	for (int j = imgB.cols*2/3;j< imgB.cols;j++)
	{
		int iNumB = 0;
		for (int i = 0; i < imgB.rows; i++)
		{
			if (imgB.at<uchar>(i, j) < 200)
			{
				iNumB++;
			}
		}
		if (iNumB > imgB.rows/10*9)
		{
			iXR = j;
		}

	}
	recRoi.x = iXL;
	recRoi.y = 0;
	recRoi.width = iXR - iXL;
	recRoi.height = imgB.rows;

	recRoi.x = recRoi.x * 10;
	recRoi.y = recRoi.y * 10;
	recRoi.width = recRoi.width * 10;
	recRoi.height = recRoi.height * 10;
	imgDst = imgDst(recRoi);




	imgB.release();
	imgG.release();
	vecPs.clear();




	return imgDst;
}

int main()
{
	Mat imgSrc, imgDst;
	Mat temp;
	string name = "..//temp//9";

	imgSrc = imread(name +".jpg");

	//int num = 1;
	//while (num < 1000)
	//{
	//	imgDst = getImgWithoutBorder(imgSrc);
	//	num++;
	//	cout << num << endl;
	//}
	imgDst = getImgWithoutBorder(imgSrc);
	imwrite(name + "½á¹û.jpg", imgDst);

}

