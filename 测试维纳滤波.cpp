#include <opencv2/opencv.hpp>
#include <iostream>
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
int main()
{
	Mat imgSrc, imgGray, imgDst;
	imgSrc = imread("3.jpg");
	cvtColor(imgSrc, imgGray, COLOR_BGR2GRAY);
	imshow("src", imgSrc);
	imshow("gray", imgGray);

	Mat kernelMat;
	kernelMat.create(Size(5, 5), CV_32FC1);
	for (int i = 0; i < kernelMat.rows; i++)
	{
		for (int j = 0; j < kernelMat.cols; j++)
		{
			kernelMat.at<float>(i, j) = 1;
		}
	}
	cout << kernelMat << endl;
	float prod = 25; 
	float sumlacalVar = 0;
	Mat imgFilter;
	filter2D(imgGray, imgFilter, CV_32FC1, kernelMat);
	//cout << imgFilter << endl;
	imshow("filter2D", imgFilter);

	Mat imgSrcF;
	imgSrc.convertTo(imgSrcF, CV_32FC1); 
	Mat imgSrcF2X;
	imgSrcF2X = imgSrcF.mul(imgSrcF);
	Mat imgSrcFilter2;
	filter2D(imgSrcF2X, imgSrcFilter2, CV_32FC1, kernelMat);
	imshow("filter2D2", imgSrcFilter2);
	waitKey(0);

	Mat imgSrcMean;
	Mat imgSrcVar;
	Mat imgDstF;
	imgSrcMean.create(imgSrc.size(), CV_32FC1);//均值
	imgSrcVar.create(imgSrc.size(), CV_32FC1);//方差
	imgDstF.create(imgSrc.size(), CV_32FC1);//结果

	for (int i = 0; i < imgSrc.rows; i++)
	{
		for (int j = 0; j < imgSrc.cols; j++)
		{
			imgSrcMean.at<float>(i, j) = imgFilter.at<float>(i, j) / prod;
			imgSrcVar.at<float>(i, j) = imgSrcFilter2.at<float>(i, j) - (imgSrcMean.at<float>(i, j)*imgSrcMean.at<float>(i, j));
			imgDstF.at<float>(i, j) = (float)imgGray.at<int>(i, j) - (float)imgSrcMean.at<int>(i, j);
		}
	}

	float fNoise = 0;
	int count = 0;
	for (int i = 0; i < imgSrc.rows; i++)
	{
		for (int j = 0; j < imgSrc.cols; j++)
		{
			fNoise = fNoise + imgSrcVar.at<float>(i, j);
			count++;
		}
	}
	fNoise = fNoise / count;//噪声
	Mat imgGrayF;
	imgGray.convertTo(imgGrayF, CV_32FC1);
	for (int i = 0; i < imgSrc.rows; i++)
	{
		for (int j = 0; j < imgSrc.cols; j++)
		{
			imgGrayF.at<float>(i, j) = imgSrcVar.at<float>(i, j) - fNoise;
		}
	}

	for (int i = 0; i < imgGrayF.rows; i++)
	{
		for (int j = 0; j < imgGrayF.cols; j++)
		{
			if (imgGrayF.at<float>(i, j) < 0)
			{
				imgGrayF.at<float>(i, j) = 0;
			}
			if (imgSrcVar.at<float>(i, j) < fNoise)
			{
				imgSrcVar.at<float>(i, j) = fNoise;
			}
		}
	}

	imgDstF = imgDstF - imgSrcVar;
	imgDstF = imgDstF.mul(imgGrayF);
	imgDstF = imgDstF + imgSrcMean;

	imgDstF.convertTo(imgDst, CV_8UC1);
	imshow("dst", imgDst);


	waitKey(0);
	return 1;
}