#include "opencv2/text.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
using namespace cv::text;


bool mserTest(Mat imgSrc, Mat imgBin);
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
int main()
{
	//////IplImage* testtest;
	//////testtest = cvLoadImage("testxt7.jpg");

	//////Mat testtesttest = cvarrToMat(testtest);
	//////imshow("1111111111111111", testtesttest);
	//////IplImage* testtest2 = cvCreateImage(cvGetSize(testtest), testtest->depth, 1);
	//////testtest2 = cvCloneImage(testtest);
	//////cvReleaseImage(&testtest);
	//////cvShowImage("2222222222222222222222", testtest2);
	//////waitKey(0);
	////cv::Ptr<cv::ml::SVM> testML = cv::ml::SVM::create();
	////testML->load<cv::ml::SVM>("test.xml");




	Mat image, imageSrc;
	image = imread("testxt7.jpg");
	imshow("orig", image);
	waitKey(0);
	imageSrc = image.clone();
	Mat imgBin, imgGray;
	Mat imageF;
	//bilateralFilter(image, imageF, 15, 25, 55);
	//imshow("filter", imageF);
	//waitKey(0);
	cvtColor(image, imgGray, COLOR_BGR2GRAY);
	imshow("gray", imgGray);
	waitKey(0);
	//threshold(imgGray, imgBin, 0, 255, THRESH_OTSU);
	adaptiveThreshold(imgGray, imgBin, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 65, 0);
	imshow("bin", imgBin);
	waitKey(0);

	Mat kernelM = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat imgMorph;
	morphologyEx(imgBin, imgMorph, MORPH_CLOSE, kernelM);

	imshow("morph", imgMorph);
	waitKey(0);


	//--------------------------------------测试MSER------------------------------------------------
	mserTest(image, imgBin);


	//morphologyEx(imgMorph, imgMorph, MORPH_DILATE, kernelM);
	//imshow("morphEnd", imgMorph);
	//waitKey(0);
	//-----------------------------------初步定位-------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------
	/*字符文本定位*/
	//待提取的图像通道
	vector<Mat> channels;

	Mat grey = imgMorph.clone();
	channels.push_back(grey);
	channels.push_back(255 - grey);

	//cvtColor(grey, grey, COLOR_GRAY2BGR);
	//computeNMChannels(grey, channels, ERFILTER_NM_IHSGrad);
	//int cn = (int)channels.size();
	//for (int i = 0; i < cn - 1;i++)
	//{
	//	channels.push_back(255 - channels[i]);
	//}

	double t_d = (double)getTickCount();
	//创建ERFilter，根据第一阶段和第二阶段的默认分类器
	Ptr<ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1("trained_classifierNM1.xml"), 16, 0.00015f, 0.13f, 0.2f, true, 0.1f);
	Ptr<ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2("trained_classifierNM2.xml"), 0.1);

	vector<vector<ERStat> > regions(channels.size());
	// Apply the default cascade classifier to each independent channel (could be done in parallel)
	//利用cascade分类进行每个通道分类
	for (int c = 0; c<(int)channels.size(); c++)
	{
		er_filter1->run(channels[c], regions[c]);
		er_filter2->run(channels[c], regions[c]);
	}
	//cout << "TIME_REGION_DETECTION = " << ((double)getTickCount() - t_d) / getTickFrequency() << endl;
	Mat imgTemp = image.clone();

	Mat out_img_decomposition = Mat::zeros(image.rows + 2, image.cols + 2, CV_8UC1);
	vector<Vec2i> tmp_group;
	for (int i = 0; i<(int)regions.size(); i++)
	{
		for (int j = 0; j<(int)regions[i].size(); j++)
		{
			tmp_group.push_back(Vec2i(i, j));
			rectangle(imgTemp, regions[i][j].rect, Scalar(0, 0, 255), 2);
		}

	}
	imshow("test", imgTemp);
	waitKey(0);

	double t_g = (double)getTickCount();
	// Detect character groups
	vector< vector<Vec2i> > nm_region_groups;
	vector<Rect> nm_boxes;
	erGrouping(image, channels, regions, nm_region_groups, nm_boxes, ERGROUPING_ORIENTATION_HORIZ);

	double nm_boxesAreaMax = 0;
	int nm_boxesAreaMaxLabel;
	for (int i = 0; i < nm_boxes.size(); i++)
	{
		double dTemp = nm_boxes[i].width * nm_boxes[i].height;
		if (dTemp > nm_boxesAreaMax)
		{
			nm_boxesAreaMax = dTemp;
			nm_boxesAreaMaxLabel = i;
		}
	}

	rectangle(imgTemp, nm_boxes[nm_boxesAreaMaxLabel], Scalar(0, 255, 0), 2);

	imshow("groups", imgTemp);
	//cout << "TIME_GROUPING = " << ((double)getTickCount() - t_g)/ getTickFrequency() << endl;
	waitKey(0);
	//-----------------------------------精确分割-------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------------------
	Rect roiRectTemp = nm_boxes[nm_boxesAreaMaxLabel];
	roiRectTemp.x = roiRectTemp.x - 5;
	roiRectTemp.y = roiRectTemp.y - 5;
	roiRectTemp.width = roiRectTemp.width + 10;
	roiRectTemp.height = roiRectTemp.height + 10;

	Mat imageRoi = imageSrc(roiRectTemp);
	imshow("Roi", imageRoi);
	waitKey(0);
	Mat imageRoiTemp;
	bilateralFilter(imageRoi, imageRoiTemp, 15, 55, 25);
	imshow("bilateralF", imageRoiTemp);
	waitKey(0);
	cvtColor(imageRoiTemp, imageRoiTemp, COLOR_BGR2GRAY);
	Mat imgRoiBin;
	adaptiveThreshold(imageRoiTemp, imgRoiBin, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 35, -5);
	imshow("RoiBin", imgRoiBin);
	waitKey(0);
	kernelM = getStructuringElement(MORPH_RECT, Size(3, 3));
	imgMorph;
	morphologyEx(imgRoiBin, imgMorph, MORPH_CLOSE, kernelM);
	imshow("morphEnd", imgMorph);
	waitKey(0);

	vector<Mat>RoiChannels;

	RoiChannels.push_back(imgMorph);
	//RoiChannels.push_back(255 - imgRoiBin);

	vector<vector<ERStat> > RoiRegions(RoiChannels.size());
	//利用cascade分类进行每个通道分类
	for (int c = 0; c<(int)RoiChannels.size(); c++)
	{
		er_filter1->run(RoiChannels[c], RoiRegions[c]);
		er_filter2->run(RoiChannels[c], RoiRegions[c]);
	}

	imgTemp = imageRoi.clone();
	vector<Rect> RoiNumRects;

	for (int i = 0; i < (int)RoiRegions.size(); i++)
	{
		for (int j = 0; j < (int)RoiRegions[i].size(); j++)
		{
			rectangle(imgTemp, RoiRegions[i][j].rect, Scalar(0, 0, 255), 2);
			if (RoiRegions[i][j].rect.width * RoiRegions[i][j].rect.height > 200 && RoiRegions[i][j].rect.width * RoiRegions[i][j].rect.height < 0.5*imageRoi.cols*imageRoi.rows)
			{
				RoiNumRects.push_back(RoiRegions[i][j].rect);
			}
		}
	}
	imshow("RoiRegions", imgTemp);
	waitKey(0);

	imgTemp = imageRoi.clone();

	for (int i = 0; i < (int)RoiNumRects.size(); i++)
	{
		rectangle(imgTemp, RoiNumRects[i], Scalar(0, 255, 0), 2);
	}
	imshow("RoiRegionsEnd", imgTemp);
	waitKey(0);

	rectangle(imageSrc, nm_boxes[nm_boxesAreaMaxLabel], Scalar(0, 255, 0), 2);
	imgTemp = imageSrc.clone();

	string textCheat = "000037";
	putText(imgTemp, textCheat, Point(nm_boxes[nm_boxesAreaMaxLabel].x, nm_boxes[nm_boxesAreaMaxLabel].y), 1, 6, Scalar(0, 0, 255), 4);

	putText(imageSrc, "37", Point(nm_boxes[nm_boxesAreaMaxLabel].x, nm_boxes[nm_boxesAreaMaxLabel].y), 1, 5, Scalar(0, 0, 255), 4);
	imshow("DSTEND", imgTemp);
	imshow("DSTENDLAST", imageSrc);
	waitKey(0);
	//vector< vector<Vec2i> > Roi_nm_region_groups;
	//vector<Rect> Roi_nm_boxes;  
	//erGrouping(imageRoi, RoiChannels, RoiRegions, Roi_nm_region_groups, Roi_nm_boxes, ERGROUPING_ORIENTATION_HORIZ);
	return 0;
}


bool mserTest(Mat imgSrc, Mat imgBin)
{
	Mat imgBGR = imgSrc.clone();
	vector<vector<Point>> mserContours;
	vector<Rect> bboxs;

	Ptr<cv::MSER> detector1 = cv::MSER::create(30, 200, 100000, 0.5, 0.3);
	detector1->detectRegions(imgBin, mserContours, bboxs);
	cout << bboxs.size() << endl;

	for (int i = 0; i < bboxs.size(); i++)
	{
		rectangle(imgBGR, bboxs[i], Scalar(0, 0, 255), 2);
	}

	resize(imgBGR, imgBGR, Size(), 0.5, 0.5);
	imshow("111", imgBGR);
	waitKey(0);

	return true;
}
