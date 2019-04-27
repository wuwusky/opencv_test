#include <opencv2/opencv.hpp>
#include <math.h>
#include <opencv2/viz.hpp>
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
Mat imgSrcAll;
int numPoint = 0;
vector<Point> vecPoints;

static void on_mouse(int event, int x, int y,int, void*)
{
	switch (event)
	{
	case EVENT_MOUSEMOVE:
	{
		break;
	}
	case EVENT_LBUTTONDOWN:
	{
		circle(imgSrcAll, Point(x, y), 2, Scalar(0, 255, 0), 2);
		numPoint++;
		if (numPoint % 2 == 0 && numPoint < 5)
		{
			line(imgSrcAll, vecPoints[numPoint - 2], Point(x, y), Scalar(0, 0, 255), 1);
		}
		if (numPoint < 100)
		{
			vecPoints.push_back(Point(x, y));
		}
		cv::imshow("原图", imgSrcAll);
		break;
	}
	case EVENT_LBUTTONUP:
	{
		break;
	}
	
		
	}
}


int main()
{
	bool flagLoopAll = true;
	vecPoints.clear();
	bool flagTest = true;
	int iMethod = 1;
	Mat imgZeros;

	if (flagLoopAll)
	{
		Mat imgSrc, imgDst, imgTemp;
		imgSrc = imread("..\\测试矫正\\2.jpg");
		if (imgSrc.cols > 1500)
		{
			resize(imgSrc, imgSrc, Size(), 0.5, 0.5);
		}
		imgTemp = imgSrc.clone();
		imgSrcAll = imgSrc.clone();
		imgZeros = Mat::zeros(imgTemp.size(), CV_8UC1);

		namedWindow("原图", 1);
		setMouseCallback("原图", on_mouse, NULL);
		cv::imshow("原图", imgSrcAll);
		cv::waitKey(0);
		RotatedRect ellipseTest;
		ellipseTest = fitEllipse(vecPoints);
		ellipse(imgTemp, ellipseTest, Scalar(0, 255, 0), 2);
		cv::imshow("拟合", imgTemp);

		cv::waitKey(0);
		cout << ellipseTest.angle << endl;
		double rotateAngle;
		if (ellipseTest.angle > 135 || ellipseTest.angle < 45)
		{
			rotateAngle = ellipseTest.angle - 180;
			flagTest = false;
		}
		else
		{
			rotateAngle = ellipseTest.angle - 90;
			flagTest = true;
		}


		Mat rotateM = getRotationMatrix2D(ellipseTest.center, rotateAngle, 1.0);
		warpAffine(imgTemp, imgTemp, rotateM, imgTemp.size());
		warpAffine(imgSrc, imgSrc, rotateM, imgTemp.size());
		cv::imshow("椭圆矫正", imgTemp);
		imwrite("椭圆矫正.jpg", imgTemp);
		cv::waitKey(0);
		cout << rotateM << endl;
		vector<Point2f> vecPEllipse;
		vector<Point2f> vecPCircle;
		double aMax, aMin;
		if (ellipseTest.size.width > ellipseTest.size.height)
		{
			aMax = ellipseTest.size.width;
			aMin = ellipseTest.size.height;
		}
		else
		{
			aMax = ellipseTest.size.height;
			aMin = ellipseTest.size.width;
		}

		cout << "长轴长:" << aMax << endl;
		cout << "短轴长:" << aMin << endl;

		if (flagTest)
		{
			vecPEllipse.push_back(Point2f(ellipseTest.center.x - aMax / 2, ellipseTest.center.y));
			vecPEllipse.push_back(Point2f(ellipseTest.center.x, ellipseTest.center.y - aMin / 2));
			vecPEllipse.push_back(Point2f(ellipseTest.center.x + aMax / 2, ellipseTest.center.y));
			vecPEllipse.push_back(Point2f(ellipseTest.center.x, ellipseTest.center.y + aMin / 2));
			vecPCircle.push_back(Point2f(ellipseTest.center.x - aMax / 2, ellipseTest.center.y));
			vecPCircle.push_back(Point2f(ellipseTest.center.x, ellipseTest.center.y - aMax / 2));
			vecPCircle.push_back(Point2f(ellipseTest.center.x + aMax / 2, ellipseTest.center.y));
			vecPCircle.push_back(Point2f(ellipseTest.center.x, ellipseTest.center.y + aMax / 2));
		}
		else
		{
			vecPEllipse.push_back(Point2f(ellipseTest.center.x - aMin / 2, ellipseTest.center.y));
			vecPEllipse.push_back(Point2f(ellipseTest.center.x, ellipseTest.center.y - aMax / 2));
			vecPEllipse.push_back(Point2f(ellipseTest.center.x + aMin / 2, ellipseTest.center.y));
			vecPEllipse.push_back(Point2f(ellipseTest.center.x, ellipseTest.center.y + aMax / 2));
			vecPCircle.push_back(Point2f(ellipseTest.center.x - aMax / 2, ellipseTest.center.y));
			vecPCircle.push_back(Point2f(ellipseTest.center.x, ellipseTest.center.y - aMax / 2));
			vecPCircle.push_back(Point2f(ellipseTest.center.x + aMax / 2, ellipseTest.center.y));
			vecPCircle.push_back(Point2f(ellipseTest.center.x, ellipseTest.center.y + aMax / 2));
		}

		switch (iMethod)
		{
		case 1:
		{
			for (int i = 0; i < vecPEllipse.size(); i++)
			{
				circle(imgTemp, vecPEllipse[i], 2, Scalar(255, 0, 0), 2);
				circle(imgTemp, vecPCircle[i], 2, Scalar(0, 0, 255), 2);
			}

			cv::imshow("选点", imgTemp);
			cv::waitKey(0);

			circle(imgZeros, ellipseTest.center, aMax / 2, 255, -1);
			cv::imshow("掩膜", imgZeros);
			cv::waitKey(0);
			//double rotateAngel = acos((double)(aMin / aMax));
			//double rotateAngelTemp;
			//rotateAngelTemp = (rotateAngel / CV_PI )* 180;
			//cout << rotateAngelTemp << endl;
			////double aaa = cos(CV_PI/3);
			////cout << aaa << endl;
			//Mat rotateM2;
			//rotateM2.create(3, 3, CV_64FC1);
			////rotateM2.at<double>(0, 0) = cos(-rotateAngel);
			////rotateM2.at<double>(0, 1) = sin(-rotateAngel);
			////rotateM2.at<double>(0, 2) = 0;
			////rotateM2.at<double>(1, 0) = -sin(-rotateAngel);
			////rotateM2.at<double>(1, 1) = cos(-rotateAngel);
			////rotateM2.at<double>(1, 2) = 0;
			////rotateM2.at<double>(2, 0) = 0;
			////rotateM2.at<double>(2, 1) = 0;
			////rotateM2.at<double>(2, 2) = 1;
			//rotateAngel = -rotateAngel;
			////double tempM[3][3] = { { rotateAngel,0,sqrt(1 - rotateAngel*rotateAngel) },{ 0,1,0 },{ -sqrt(1 - rotateAngel*rotateAngel),0,rotateAngel } };
			//double tempM[3][3] = { {1, 0, -0}, {0,1, -159}, {0, 0, 1} };
			//for (int i = 0; i < 3; i++)
			//{
			//	for (int j = 0; j < 3; j++)
			//	{
			//		rotateM2.at<double>(i, j) = tempM[i][j];
			//	}
			//}
			//cout << "---------------------------------" << endl;
			//cout << rotateM2 << endl;

			Mat rotateM2;
			Mat rotateTemp;
			rotateM2.create(3, 3, CV_64FC1);
			rotateTemp.create(3, 3, CV_64FC1);
			rotateM2 = getPerspectiveTransform(vecPEllipse, vecPCircle);
			for (int i = 0; i < vecPEllipse.size(); i++)
			{
				cout << "vecPEllipse:" << vecPEllipse[i] << endl;
				cout << "vecPCircle:" << vecPCircle[i] << endl;
			}

			Mat maskTemp;
			rotateTemp  = findHomography(vecPEllipse, vecPCircle, maskTemp, 0, 3.0);
			cout << "getPerspectiveTransform:"  << endl << rotateM2 << endl;
			cout << "findHomography:" << endl << rotateTemp << endl;
			warpPerspective(imgSrc, imgDst, rotateM2, imgTemp.size());
			Mat imgDstTemp;
			warpPerspective(imgSrc, imgDstTemp, rotateTemp, imgTemp.size());



			//Mat rotateMBack = getRotationMatrix2D(ellipseTest.center, -rotateAngle, 1.0);
			//warpAffine(imgDst, imgDst, rotateMBack, imgDst.size());
			//warpAffine(imgZeros, imgZeros, rotateMBack, imgZeros.size());
			//warpAffine(imgDstTemp, imgDstTemp, rotateMBack, imgDstTemp.size());





			cv::imshow("结果", imgDst);
			cv::imshow("结果2", imgDstTemp);
			imwrite("结果.jpg", imgDst);
			cv::waitKey(0);


			//viz::Viz3d myWindow("测试");
			//myWindow.showWidget("Coordinate", viz::WCoordinateSystem());
			////myWindow.showImage(imgSrc);

			//Affine3d ssss;
			//ssss = myWindow.getViewerPose();
			//cout << ssss.matrix << endl;
			//ssss.matrix.zeros();
			//myWindow.setViewerPose(ssss);
			//cout << ssss.matrix << endl;
			//myWindow.spin();
			////myWindow.spinOnce(1, true);
			////while (!myWindow.wasStopped())
			////{
			////	myWindow.spinOnce(1, true);
			////}



			//Mat rotateM11 = getRotationMatrix2D(ellipseTest.center, 30, 1.0);
			//warpAffine(imgDst, imgDst, rotateM11, imgDst.size());
			//warpAffine(imgZeros, imgZeros, rotateM11, imgZeros.size());
			//imshow("test", imgDst);
			//waitKey(0);
			Mat imgDstEnd;
			imgDst.copyTo(imgDstEnd, imgZeros);
			cv::imshow("最终结果", imgDstEnd);
			cv::waitKey(0);
			imwrite("最终结果.jpg", imgDstEnd);
			imgDstTemp.copyTo(imgDstEnd, imgZeros);
			cv::imshow("最终结果2", imgDstEnd);
			cv::waitKey(0);
			//-------------------------水平矫正-------------------------------------
			//Mat imgDstTemp = imgDstEnd.clone();
			//threshold(imgDstTemp, imgDstTemp, 0, 255, THRESH_OTSU);
			//imshow("OTSU", imgDstTemp);
			//waitKey(0);

			break;
		}
		case 2:
		{
			vector<Point2f> vecPRect;
			vector<Point2f> vecPTrape;
			vecPTrape.push_back(Point2f(vecPEllipse[1].x - aMin / 2, vecPEllipse[1].y));
			vecPTrape.push_back(Point2f(vecPEllipse[1].x + aMin / 2, vecPEllipse[1].y));
			vecPTrape.push_back(Point2f(vecPEllipse[2].x, vecPEllipse[0].y + aMin / 2));
			vecPTrape.push_back(Point2f(vecPEllipse[0].x, vecPEllipse[2].y + aMin / 2));

			vecPRect.push_back(Point2f(vecPTrape[3].x, vecPTrape[3].y - aMax));
			vecPRect.push_back(Point2f(vecPTrape[2].x, vecPTrape[2].y - aMax));
			vecPRect.push_back(vecPTrape[2]);
			vecPRect.push_back(vecPTrape[3]);

			for (int i = 0; i < 3; i++)
			{
				line(imgTemp, vecPTrape[i], vecPTrape[i + 1], Scalar(0, 255, 0), 2);
				line(imgTemp, vecPRect[i], vecPRect[i + 1], Scalar(255, 0, 0), 2);
			}
			line(imgTemp, vecPTrape[3], vecPTrape[0], Scalar(0, 255, 0), 2);
			line(imgTemp, vecPRect[3], vecPRect[0], Scalar(255, 0, 0), 2);
			cv::imshow("梯形和矩形", imgTemp);
			cv::waitKey(0);

			Mat rotateM2;
			rotateM2.create(3, 3, CV_32FC1);
			rotateM2 = getPerspectiveTransform(vecPTrape, vecPRect);
			cout << rotateM2 << endl;
			warpPerspective(imgTemp, imgDst, rotateM2, imgTemp.size());
			cv::imshow("结果", imgDst);
			cv::waitKey(0);


			break;
		}
		}



	}
	else
	{
		//-------------------------水平矫正-------------------------------------
		Mat imgDstTemp;
		imgDstTemp = imread("..\\测试矫正\\7最终结果.jpg", 0);
		cv::imshow("结果原图", imgDstTemp);
		cv::adaptiveThreshold(imgDstTemp, imgDstTemp, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 97, 0);
		//imshow("OTSU", imgDstTemp);
		Mat imgRectTemp = imgDstTemp.clone();


		vector<vector<Point>> contours;
		findContours(imgRectTemp, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
		cout << contours.size() << endl;

		int labelMax;
		double areaMax = 0;
		for (int i = 0; i < contours.size(); i++)
		{
			double temp;
			temp = contourArea(contours[i]);
			if (areaMax < temp)
			{
				areaMax = temp;
				labelMax = i;
			}

		}
		//RotatedRect rectTemp = minAreaRect(contours[0]);
		Rect testtest = boundingRect(contours[labelMax]);
		Mat imgSrcTemp = imread("..\\测试矫正\\7结果.jpg");
		Mat imgRoi = imgSrcTemp(testtest);
		cv::imshow("ROI", imgRoi);
		cv::waitKey(0);
		Mat temp;
		cv::bilateralFilter(imgRoi, temp, 15, 55, 25);
		cv::imshow("滤波", temp);
		cv::waitKey(0);
		cv::cvtColor(temp, imgRoi, COLOR_BGR2GRAY);
		cv::adaptiveThreshold(imgRoi, imgRoi, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 85, 0);
		cv::imshow("OTSU", imgRoi);
		cv::waitKey(0);
		Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
		//morphologyEx(imgRoi, imgRoi, MORPH_DILATE, kernel, Point(-1, -1), 1);
		cv::morphologyEx(imgRoi, imgRoi, MORPH_ERODE, kernel, Point(-1, -1), 2);
		cv::imshow("腐蚀下", imgRoi);
		cv::waitKey(0);


	}





	return 1;
}