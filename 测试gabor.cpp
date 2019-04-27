#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "myGabor.h"
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void myGabor1(Mat &image, Size kSize, double sigma, double gamma, double psi);
void myGaborT(Mat &image, Size kSize, double theta, double sigma);
//--------------------------------------------------------------------------------------
void myGabor1(Mat &image, Size kSize, double sigma, double gamma, double psi)
{
	int ktype = CV_32F;
	Mat imgsShow(Size(800, 500), CV_8U);

	for (int i = 0; i < 5; i ++)
	{
		for (int j = 0; j < 8; j ++)
		{
			Mat gaborKernel = getGaborKernel(kSize, sigma, j*CV_PI/8 + CV_PI/2, i, gamma, psi);
			Mat temp2;
			resize(image, temp2, Size(100, 100));

			filter2D(temp2, temp2, CV_8U, gaborKernel);

			Mat temp;
			//equalizeHist(temp2, temp);
			normalize(temp2, temp, 0, 255, NORM_MINMAX);
			temp.copyTo(imgsShow(Rect(100*j, 100*i, 100, 100)));
		}
	}
	imshow("4", imgsShow);
	waitKey(0);	

}
//--------------------------------------------------------------------------------------
void myGaborT(Mat &image, Size kSize, double theta, double sigma)
{
	Mat dst;
	Mat gaborKernel = getGaborKernel(kSize, sigma, theta, 2, -1);
	filter2D(image, dst, CV_8U, gaborKernel);

	//normalize(dst, dst, 0, 255, NORM_MINMAX);
	//equalizeHist(dst, dst);
	Mat temp = dst;
	resize(dst, temp, Size(600, 350));
	imshow("1", temp);
	waitKey(0);

}
//--------------------------------------------------------------------------------------
int main()
{
	Mat src = imread("D:\\computer visio\\测试导向滤波\\测试导向滤波\\test/1 (9).jpg", 0);
	Mat dst;
	//Size ksize = Size(src.cols/2, src.rows/2);
	//double sigma = 2*CV_PI;
	////double theta ;//方向角度
	////double lambd ;//正弦波长
	//double gamma = 1;
	//double psi = CV_PI*0.5;



	//myGaborT(src, Size(src.cols/2,src.rows/2), PI/8 * 0, 2*PI);

	//CvGabor myTestGabor;
	//myTestGabor.Init(PI/8 * 1, 1, 2*PI, PI/2);
	////myTestGabor.creat_kernel();
	//IplImage* iplSrc = cvLoadImage("模板1.jpg", 0);
	//IplImage* iplDst = iplSrc;
	//myTestGabor.conv_img(iplSrc, iplDst, CV_GABOR_MAG);
	//IplImage* iplTemp = cvCreateImage(CvSize(600, 350),iplSrc->depth ,1);
	//cvResize(iplDst, iplTemp);
	//cvShowImage("2", iplTemp);
	//waitKey(0);
	//myGabor(src, ksize, sigma, gamma, psi);
	//src = imread("g2.jpg", 0);
	//myGabor(src, ksize, sigma, gamma, psi);

	myGabor testGabor;
	Mat imgsRShow(Size(800, 500), CV_32FC1);
	Mat imgsimgShow(Size(800, 500), CV_32FC1);
	Mat imgsPShow(Size(800, 500), CV_32FC1);
	Mat imgsMShow(Size(800, 500), CV_32FC1);
	int iSizeX = 15;
	int iSizeY = 9;
	Mat dstAvg;


	for (int i = 1; i < 6; i ++)
	{
		for (int j = 0; j < 8; j ++)
		{
			Mat tempR, tempI, tempPhase, tempMain;


			Mat kernel = testGabor.getRealGaborKernel(Size(iSizeX, iSizeY), 2*CV_PI, j * CV_PI/8 + CV_PI/2, i, 1);
			tempR = testGabor.getFilterRealPart(src, kernel);

			kernel = testGabor.getImagGaborKernel(Size(iSizeX, iSizeY), 2*CV_PI, j * CV_PI/8+ CV_PI/2, i, 1);
			tempI = testGabor.getFilterImagPart(src, kernel);
			tempPhase = testGabor.getPhase(tempR, tempI);
			tempMain = testGabor.getMagnitude(tempR, tempI);
			Mat temp = tempMain.clone();
			//temp.convertTo(temp, CV_64FC1);
			//dstAvg = Mat::zeros(temp.size(), CV_64FC1);
			//dstAvg = dstAvg + temp;

			resize(tempR, tempR, Size(100, 100));
			tempR.copyTo(imgsRShow(Rect(100*j, 100*(i - 1), 100, 100)));

			resize(tempI, tempI, Size(100, 100));
			tempI.copyTo(imgsimgShow(Rect(100*j, 100*(i-1), 100, 100)));

			resize(tempPhase, tempPhase, Size(100, 100));
			tempPhase.copyTo(imgsPShow(Rect(100*j, 100*(i-1), 100, 100)));

			resize(tempMain, tempMain, Size(100, 100));
			tempPhase.copyTo(imgsMShow(Rect(100*j, 100*(i-1), 100, 100)));

		}
	}
	//normalize(dstAvg, dstAvg, 0, 255, NORM_MINMAX, CV_8U);
	cv::imshow("实部图像", imgsRShow);
	cv::imshow("2", imgsimgShow);
	cv::imshow("相值", imgsPShow);
	cv::imshow("幅值", imgsMShow);
	//cv::imshow("平均图",dstAvg);
	cv::waitKey(0);
	//system("pause");
}