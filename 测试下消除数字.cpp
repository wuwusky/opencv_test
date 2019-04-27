#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc/edge_filter.hpp>
//-------------------------------------------------------------//
//-------------------------------------------------------------//
using namespace cv;
using namespace std;
using namespace ximgproc;
//-------------------------------------------------------------//
//-------------------------------------------------------------//
int main()
{
	Mat src, dst, srcGuide, srcGray;

	double t = getTickCount();

	src = imread("3.jpg",1);
	//resize(src,src,Size(800,500));
	cvtColor(src,srcGray,COLOR_BGR2GRAY);
	threshold(srcGray,srcGuide,100,255,THRESH_OTSU);
	guidedFilter(src,src,dst,100,50);

	imshow("原图",src);
	////imshow("灰度图",srcGray);
	//imshow("二值图",srcGuide);
	//bitwise_not(srcGuide,srcGuide);
	//imshow("二值图取反",srcGuide);
	//src.copyTo(srcGray, srcGuide);
	imshow("导向滤波",dst);
	imshow("test",srcGray);

	t = ((double) getTickCount() - t)/getTickFrequency();
	cout << "导向滤波耗时：" <<t<<endl;

	t = getTickCount();
	//bilateralFilter(src, dst, 100, 25*2, 25/2);
	imshow("双边滤波",dst);
	t = ((double)getTickCount() - t)/getTickFrequency();
	cout << "双边滤波耗时" <<t<<endl;

	waitKey(0);
}