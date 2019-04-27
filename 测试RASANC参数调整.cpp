#include "opencv2/opencv.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/features2d/features2d.hpp"
//-----------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//-----------------------------------------------------------------------------------------
void test(int nIters, float fCoffe, int ranValue);

void test(int nIters, float fCoffe, int ranValue)
{
	Mat dstImage;
	Mat image, imageTmp;
	string addr1 = "C:\\Users\\wuwuw\\Desktop\\图片\\133-1\\测试.jpg";
	string addr2 = "C:\\Users\\wuwuw\\Desktop\\图片\\133-1\\模板.jpg";
	image = imread(addr1, 0);
	imageTmp = imread(addr2, 0);

	vector<KeyPoint> imgKeyPoints, imgTmpKeyPoints;
	//FastFeatureDetector detector(5);
	//const Ptr<AKAZE> &featureDetector = AKAZE::create();
	//detector.detect(image, imgKeyPoints);
	//detector.detect(imageTmp, imgTmpKeyPoints);
	const Ptr<FastFeatureDetector> &featureDetector = FastFeatureDetector::create(5);
	
	const Ptr<BRISK> &featureExtractor = BRISK::create();

	Mat imgDescriptors, imgTmpDescriptors;

	//featureExtactor.compute(image, imgKeyPoints, imgDescriptors);
	//featureExtactor.compute(imageTmp, imgTmpKeyPoints, imgTmpDescriptors);
	featureDetector->detect(image, imgKeyPoints);
	featureDetector->detect(imageTmp, imgTmpKeyPoints);
	featureExtractor->compute(image, imgKeyPoints, imgDescriptors);
	featureExtractor->compute(imageTmp, imgTmpKeyPoints, imgTmpDescriptors);

	//featureDetector->compute(image, imgKeyPoints, imgDescriptors);
	//featureDetector->compute(imageTmp, imgTmpKeyPoints, imgTmpDescriptors);


	BFMatcher matcher(NORM_HAMMING, true);
	vector<DMatch> matches;

	matcher.match(imgTmpDescriptors, imgDescriptors, matches);


	double max_dist = INT_MIN;
	double min_dist = INT_MAX;
	vector<DMatch> good_matches;

	for (int i = 0; i < matches.size(); i ++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist)
		{
			min_dist = dist;
		}
		if (dist > max_dist)
		{
			max_dist = dist;
		}
	}
	//float minDist[10];
	//for (int i = 0; i < 10; i ++)
	//{
	//	minDist[i] = matches[i].distance;
	//}

	//for (int i = 9; i < matches.size(); i ++)
	//{
	//	for (int j = 0; j < 9; j ++)
	//	{

	//	}
	//}
	for (int i = 0; i < matches.size(); i ++)
	{
		if (matches[i].distance <= 5 * min_dist && matches[i].distance <= 500)
		{
			good_matches.push_back(matches[i]);
		}
	}
	//drawMatches(imageTmp, imgTmpKeyPoints, image, imgKeyPoints, good_matches, dstImage);
	//resize(dstImage, dstImage, Size(1000, 500));
	//imshow("初步配对情况",dstImage);


	vector<Point2f> img, imgTmp;
	vector<uchar>matchPairMask;

	for (int i = 0; i < good_matches.size(); i ++)
	{
		img.push_back(imgKeyPoints[good_matches[i].trainIdx].pt);
		imgTmp.push_back(imgTmpKeyPoints[good_matches[i].queryIdx].pt);
	}

	Mat H = findHomography(imgTmp, img, RANSAC, ranValue, matchPairMask, nIters, fCoffe);
	//Mat H = findHomography(imgTmp, img, CV_FM_RANSAC, 3 );
	vector<DMatch> good_matchesEnd;
	for (int i = 0; i < good_matches.size(); i ++)
	{
		if(matchPairMask[i] == 1)
		{
			good_matchesEnd.push_back(good_matches[i]);
		}
	}

	drawMatches(imageTmp, imgTmpKeyPoints, image, imgKeyPoints, good_matchesEnd, dstImage);
	resize(dstImage, dstImage, Size(1000, 500));
	imshow("最终配对情况",dstImage);

	cout <<"ransac阈值"<<ranValue<<endl;
	cout <<"置信度："<<fCoffe<<endl;
	cout <<"迭代次数："<<nIters<<endl;
	cout << good_matches.size()<<endl;
	cout <<good_matchesEnd.size()<<endl;

	cv::waitKey(0);

}

void main()
{
	for (float i = 0.1; i < 1; i = i + 0.1)
	{
		test(2000, i, 10);
	}
	//for (float i = 0; i < 11; i = i + 1)
	//{
	//	test(2000, 0.9, i);
	//}


	system("pause");
}