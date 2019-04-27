#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void myNiBlackThreshold(InputArray _src, OutputArray _dst, double maxValue, int type, int blockSize, double delta);
//--------------------------------------------------------------------------------------
void myNiBlackThreshold(InputArray _src, OutputArray _dst, double maxValue, int type, int blockSize, double delta)
{
	Mat src = _src.getMat();
	CV_Assert(src.type() == CV_8UC1);
	CV_Assert(blockSize % 2 == 1 && blockSize > 1);
	Size size = src.size();

	_dst.create(size, src.type());
	Mat dst = _dst.getMat();

	if (maxValue < 0)
	{
		dst = Scalar(0);
		return;
	}

	// Calculate and store the mean and mean of squares in the neighborhood
	// of each pixel and store them in Mat mean and sqmean.
	//Mat_<float> mean(size), sqmean(size);

	////if (src.data != dst.data)
	//	mean = dst;
	Mat mean, sqmean;

	boxFilter(src, mean, CV_64FC1, Size(blockSize, blockSize),Point(-1, -1), true, BORDER_REPLICATE);
	sqrBoxFilter(src, sqmean, CV_64FC1, Size(blockSize, blockSize), Point(-1, -1), true, BORDER_REPLICATE);

	// Compute (k * standard deviation) in the neighborhood of each pixel
	// and store in Mat stddev. Also threshold the values in the src matrix to compute dst matrix.
	//Mat_<float> stddev(size);
	Mat stddev = mean;
	int i, j, threshold;
	uchar imaxval = saturate_cast<uchar>(maxValue);
	for (i = 0; i < size.height; ++i)
	{
		for (j = 0; j < size.width; ++j)
		{
			stddev.at<double>(i, j) = delta * cvRound(sqrt(sqmean.at<double>(i, j) - mean.at<double>(i, j)*mean.at<double>(i, j)));
			threshold = cvRound(mean.at<double>(i, j) + stddev.at<double>(i, j));
			if (src.at<uchar>(i, j) > threshold)
				dst.at<uchar>(i, j) = (type == THRESH_BINARY) ? imaxval : 0;
			else
				dst.at<uchar>(i, j) = (type == THRESH_BINARY) ? 0 : imaxval;
		}
	}

}
//--------------------------------------------------------------------------------------
void main()
{
	int iKsize = 105;
	Mat src = imread("33_1 - ¸±±¾.jpg", 0);
	src.convertTo(src, CV_8UC1);
	Mat dst1, dst2, dst3;
	imshow("0", src);

	myNiBlackThreshold(src, dst1, 255, THRESH_BINARY, iKsize, 1);
	imshow("1", dst1);
	adaptiveThreshold(src, dst2, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, iKsize, 10);
	imshow("2", dst2);
	threshold(src, dst3, 0, 255, THRESH_OTSU);
	imshow("3", dst3);
	
	waitKey(0);
}

