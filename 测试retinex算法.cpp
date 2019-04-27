#include <highgui.h>
#include <iostream>
//#include <math.h>
#include <opencv2\opencv.hpp>
#include <opencv2/core.hpp>
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
vector<float> vals;
vector<float> vals2;
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
//单尺度Retinex图像增强程序
//src为待处理图像
//sigma为高斯模糊标准差
//scale为对比度系数
void SSR(Mat src, Mat& dst, int sigma, int scale);
//void SSR2(IplImage* src, int sigma, int scale);
//--------------------------------------------------------------------------------------
void SSR(Mat src, Mat& dst, int sigma, int scale)
{
	Mat src_f1, src_f2, src_f3;
	src.convertTo(src_f1, CV_32FC3, 1.0, 1.0);//转换范围，所有图像元素增加1.0保证cvlog正常
	cv::log(src_f1, src_f2);//Log(I(x,y))

	GaussianBlur(src_f1 ,src_f3, Size(0, 0), sigma);//Gauss(I(x,y)
	
	cv::log(src_f3, src_f3);//Log(Gauss(I(x,y))

	cv::subtract(src_f2, src_f3, src_f1);//Retinex公式，Log(R(x,y))=Log(I(x,y))-Log(Gauss(I(x,y)))

	//计算图像的均值、方差,利用均值方差等信息进行变换
	Scalar mean;
	Scalar dev;
	cv::meanStdDev(src_f1, mean, dev);
	double min[3];
	double max[3];
	double maxmin[3];
	for (size_t i = 0; i < 3; i++)
	{
		min[i] = mean.val[i] - scale * dev.val[i];
		max[i] = mean.val[i] + scale * dev.val[i];
		maxmin[i] = max[i] - min[i];
	}
	//cout << min[0] << ";" << min[1] << ";"<<min[2]<< endl;
	//cout << max[0] << ";" << max[1] << ";" << max[2] << endl;
	//cout << maxmin[0] << ";" << maxmin[1] << ";" << maxmin[2] << endl;
	//cout << "--------------------------------------------------------------" << endl;
	for (int i = 0; i < src_f3.rows; i++)
	{
		for (int j = 0; j < src_f3.cols; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				double a = 255 * (src_f1.at<Vec3f>(i, j)[k] - min[k]) / maxmin[k];
				src_f1.at<Vec3f>(i, j)[k] = a;
				vals.push_back(a);
				//cout << a << endl;
			}
		
		}
	}
	
	src_f1.convertTo(dst, CV_32FC3, 1.0, 0.0);

}


//void SSR2(IplImage* src, int sigma, int scale)
//{
//	IplImage* src_fl = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, src->nChannels);
//	IplImage* src_fl1 = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, src->nChannels);
//	IplImage* src_fl2 = cvCreateImage(cvGetSize(src), IPL_DEPTH_32F, src->nChannels);
//	float a = 0.0, b = 0.0, c = 0.0;
//	cvConvertScale(src, src_fl, 1.0, 1.0);//转换范围，所有图像元素增加1.0保证cvlog正常
//	cvLog(src_fl, src_fl1);
//
//	cvSmooth(src_fl, src_fl2, CV_GAUSSIAN, 0, 0, sigma);        //SSR算法的核心之一，高斯模糊
//
//	cvLog(src_fl2, src_fl2);
//	cvSub(src_fl1, src_fl2, src_fl);//Retinex公式，Log(R(x,y))=Log(I(x,y))-Log(Gauss(I(x,y)))
//
//	//计算图像的均值、方差，SSR算法的核心之二
//	//使用GIMP中转换方法：使用图像的均值方差等信息进行变换
//	//没有添加溢出判断
//	CvScalar mean;
//	CvScalar dev;
//	cvAvgSdv(src_fl, &mean, &dev, NULL);//计算图像的均值和标准差
//	double min[3];
//	double max[3];
//	double maxmin[3];
//	for (int i = 0; i<3; i++)
//	{
//		min[i] = mean.val[i] - scale*dev.val[i];
//		max[i] = mean.val[i] + scale*dev.val[i];
//		maxmin[i] = max[i] - min[i];
//	}
//
//	cout << min[0] << ";" << min[1] << ";" << min[2] << endl;
//	cout << max[0] << ";" << max[1] << ";" << max[2] << endl;
//	cout << maxmin[0] << ";" << maxmin[1] << ";" << maxmin[2] << endl;
//
//	float* data2 = (float*)src_fl->imageData;
//	for (int i = 0; i<src_fl2->height; i++)
//	{
//		for (int j = 0; j<src_fl2->width; j++)
//		{
//			for (int k = 0; k < 3; k++)
//			{
//				double b = 255 * (data2[i*src_fl->widthStep / 4 + 3 * j + k] - min[k]) / maxmin[k];
//				data2[i*src_fl->widthStep / 4 + 3 * j + k] = b;
//				vals2.push_back(b);
//			}
//		}
//	}
//
//
//	cvConvertScale(src_fl, src, 1, 0);
//	cvReleaseImage(&src_fl);
//	cvReleaseImage(&src_fl1);
//	cvReleaseImage(&src_fl2);
//}


void main()
{
        Mat src=imread("D:\\迅雷下载\\non-blind-deconv(1)\\44.jpg");
		Mat dst;
		int iSigma = 5;
		int iScale = 2;

		while (iScale < 100)
		{
			SSR(src, dst, iSigma, 10);

			Mat temp;
			dst.convertTo(temp, CV_8UC3);

			imshow("原图", src);
			imshow("处理后", temp);

			//IplImage* frog;
			//frog = &IplImage(src);
			//SSR2(frog, 30, 5);
			//cvShowImage("老接口处理后", frog);
			waitKey(0);

			cout << "sigma:" << iSigma << endl;
			iSigma += 2;			
		}


}