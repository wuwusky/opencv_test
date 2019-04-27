#include<opencv2/opencv.hpp>
#include<iostream>
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int main()
{
	Mat imgSrc = imread("15-2.jpg", 0);
	imshow("原图", imgSrc);
	waitKey(0);
	Mat imgTemp;

	for (int i=0;i< 7;i++)
	{
		if (i!=5 && i!= 6)
		{
			cout << i << endl;
			resize(imgSrc, imgTemp, Size(), 4.0, 4.0, i);
			imshow("插值图", imgTemp);

			equalizeHist(imgTemp, imgTemp);
			imshow("均衡化", imgTemp);
			waitKey(0);
		}

	}

	waitKey(0);
	destroyAllWindows();
	return 1;
}