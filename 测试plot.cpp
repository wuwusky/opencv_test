#include "opencv2/opencv.hpp"
#include "opencv2/plot.hpp"
#include <fstream>
//-----------------------------------------------------------------------//
//-----------------------------------------------------------------------//
using namespace cv;
using namespace std;
//-----------------------------------------------------------------------//
//-----------------------------------------------------------------------//
int main()
{
	fstream file("distance1000.txt", ios::in);
	int dis;
	vector<int> vecDistance;
	while (!file.eof())
	{
		file >> dis;
		vecDistance.push_back(dis);
	}


	Mat data(1, vecDistance.size(), CV_64FC1);
	for (size_t i = 0; i < vecDistance.size(); i++)
	{
		data.at<double>(0, i) = (double)vecDistance[i]/10000;
	}
	Mat data500(1, vecDistance.size()/2, CV_64FC1);
	for (size_t i = 0; i < vecDistance.size()/2; i++)
	{
		data.at<double>(0, i) = (double)vecDistance[i]/10000;
	}
	Mat data200(1, vecDistance.size() / 5, CV_64FC1);
	for (size_t i = 0; i < vecDistance.size() / 5; i++)
	{
		data.at<double>(0, i) = (double)vecDistance[i] / 10000;
	}
	Mat data50(1, vecDistance.size() / 20, CV_64FC1);
	for (size_t i = 0; i < vecDistance.size() / 20; i++)
	{
		data.at<double>(0, i) = (double)vecDistance[i] / 10000;
	}




	Ptr<plot::Plot2d> myPlot = plot::createPlot2d(data);
	Mat dataPlot;
	myPlot->setMaxY(200);
	myPlot->setMinY(0);
	myPlot->setMaxX(1000);
	myPlot->setMinX(0);
	myPlot->setPlotAxisColor(Scalar(0, 0, 255));//坐标轴颜色
	myPlot->setPlotBackgroundColor(Scalar(0, 255, 0));//背景颜色
	myPlot->setPlotGridColor(Scalar(0, 0, 0));//网格颜色
	myPlot->setPlotLineColor(Scalar(0, 255, 255));//曲线颜色
	myPlot->setPlotLineWidth(1.5);//所有线条宽度
	myPlot->setPlotSize(400, 300);//曲线表大小，最小是400*300
	myPlot->render(dataPlot);

	imshow("1000次", dataPlot);


	Ptr<plot::Plot2d> myPlot500 = plot::createPlot2d(data500);
	Mat dataPlot500;
	myPlot->setMaxY(200);
	myPlot->setMinY(0);
	myPlot->setMaxX(500);
	myPlot->setMinX(0);
	myPlot->setPlotAxisColor(Scalar(0, 0, 255));//坐标轴颜色
	myPlot->setPlotBackgroundColor(Scalar(0, 255, 0));//背景颜色
	myPlot->setPlotGridColor(Scalar(0, 0, 0));//网格颜色
	myPlot->setPlotLineColor(Scalar(0, 255, 255));//曲线颜色
	myPlot->setPlotLineWidth(1.5);//所有线条宽度
	myPlot->setPlotSize(400, 300);//曲线表大小，最小是400*300
	myPlot->render(dataPlot500);

	imshow("500次", dataPlot500);


	Ptr<plot::Plot2d> myPlot200 = plot::createPlot2d(data200);
	Mat dataPlot200;
	myPlot->setMaxY(200);
	myPlot->setMinY(0);
	myPlot->setMaxX(200);
	myPlot->setMinX(0);
	myPlot->setPlotAxisColor(Scalar(0, 0, 255));//坐标轴颜色
	myPlot->setPlotBackgroundColor(Scalar(0, 255, 0));//背景颜色
	myPlot->setPlotGridColor(Scalar(0, 0, 0));//网格颜色
	myPlot->setPlotLineColor(Scalar(0, 255, 255));//曲线颜色
	myPlot->setPlotLineWidth(1.5);//所有线条宽度
	myPlot->setPlotSize(400, 300);//曲线表大小，最小是400*300
	myPlot->render(dataPlot200);

	imshow("200次", dataPlot200);

	Ptr<plot::Plot2d> myPlot50 = plot::createPlot2d(data50);
	Mat dataPlot50;
	myPlot->setMaxY(200);
	myPlot->setMinY(0);
	myPlot->setMaxX(50);
	myPlot->setMinX(0);
	myPlot->setPlotAxisColor(Scalar(0, 0, 255));//坐标轴颜色
	myPlot->setPlotBackgroundColor(Scalar(0, 255, 0));//背景颜色
	myPlot->setPlotGridColor(Scalar(0, 0, 0));//网格颜色
	myPlot->setPlotLineColor(Scalar(0, 255, 255));//曲线颜色
	myPlot->setPlotLineWidth(1.5);//所有线条宽度
	myPlot->setPlotSize(400, 300);//曲线表大小，最小是400*300
	myPlot->render(dataPlot50);

	imshow("50次", dataPlot50);

	//Mat data2 = (Mat_<double>(1, 10) << 5, 4, 3 ,2,1,0, 0,1,2,3);
	//Ptr<plot::Plot2d> myPlot2 = plot::createPlot2d(data, data2);
	//Mat dataPlot2;
	//myPlot2->setMaxY(10);
	//myPlot2->setMinY(0);
	//myPlot2->setMaxX(10);
	//myPlot2->setMinX(0);
	//myPlot2->setPlotAxisColor(Scalar(0, 255, 255));//坐标轴颜色
	//myPlot2->setPlotBackgroundColor(Scalar(0, 255, 0));//背景颜色
	//myPlot2->setPlotGridColor(Scalar(0, 0, 0));//网格颜色
	//myPlot2->setPlotLineColor(Scalar(0, 255, 255));//曲线颜色
	//myPlot2->setPlotLineWidth(1.5);//所有线条宽度
	//myPlot2->setPlotSize(500, 500);//曲线表大小，最小是400*300
	//myPlot2->render(dataPlot2);

	//imshow("222", dataPlot2);
	//imwrite("plot测试.jpg", dataPlot);
	waitKey(0);
	return 1;
}
