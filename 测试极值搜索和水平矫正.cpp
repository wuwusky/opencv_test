#include <opencv2/opencv.hpp>
#include <opencv2/text.hpp>
#include <iostream>
#include <opencv2/tracking.hpp>
#include <opencv2/surface_matching.hpp>
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
void getRoiImg(Mat imgSrc, Mat& imgRoi);
//-----------------------------------------------------------------------------------

int main()
{
	Mat imgSrc, imgDst;
	imgSrc = imread("..\\testEllipse\\2最终结果1.jpg", 1);
	if (imgSrc.cols > 1500)
	{
		resize(imgSrc, imgSrc, Size(), 0.5, 0.5);
	}
	imshow("原图", imgSrc);
	Mat imgTemp = imgSrc.clone();
	waitKey(0);
	Mat imgRoi;
	getRoiImg(imgSrc, imgRoi);
	imshow("ROI", imgRoi);
	waitKey(0);
	Mat imgRoiPro;
	bilateralFilter(imgRoi, imgRoiPro, 5, 55, 25);
	imshow("滤波", imgRoiPro);
	waitKey(0);


	////----------------------------------极值搜索--------------------------------------------------------------------------
	//vector<Mat> channels;
	//cv::text::computeNMChannels(imgRoiPro, channels, cv::text::ERFILTER_NM_RGBLGrad);
	//Ptr<cv::text::ERFilter> er_filter1 = cv::text::createERFilterNM1(cv::text::loadClassifierNM1("trained_classifierNM1.xml"), 16, 0.00015f, 0.13f, 0.2f, true, 0.1f);
	//Ptr<cv::text::ERFilter> er_filter2 = createERFilterNM2(cv::text::loadClassifierNM2("trained_classifierNM2.xml"), 0.1);
	//vector<vector<cv::text::ERStat>> regions(channels.size());
	//vector<vector<cv::text::ERStat>> regionsMy(1);

	//for (int i = 0; i < channels.size(); i++)
	//{
	//	er_filter1->run(channels[i], regions[i]);
	//	er_filter2->run(channels[i], regions[i]);
	//}

	//vector<Rect> vecNums;
	//for (int i = 0; i < regions.size(); i++)
	//{
	//	for (int j = 0; j < regions[i].size(); j++)
	//	{

	//		Rect rectNum = regions[i][j].rect;
	//		regionsMy[0].push_back(regions[i][j]);

	//		if ((float)((float)rectNum.height/(float)rectNum.width) > 1.5 && (float)((float)rectNum.height/(float)rectNum.width) < 2.5 && rectNum.width < imgRoi.cols/3 && rectNum.height < imgRoi.rows/3)
	//		{
	//			if (regions[i][j].probability > 0.99 && rectNum.height > imgRoi.rows/50)
	//			{
	//				rectangle(imgRoiPro, rectNum, Scalar(0, 255, 0), 1);
	//				cout << regions[i][j].probability << endl;
	//			}
	//			else
	//			{
	//				rectangle(imgRoiPro, rectNum, Scalar(0, 0, 255), 1);

	//			}
	//		}

	//	}
	//}

	//imshow("极值搜索", imgRoiPro);
	//waitKey(0);
	//vector<vector<Vec2i>>temp;
	//vector<Rect> groupRects;
	//channels.clear();
	//Mat tempMat;
	//cvtColor(imgRoi, tempMat, COLOR_BGR2GRAY);
	//channels.push_back(tempMat);
	//
	//cv::text::erGrouping(imgRoiPro, channels, regionsMy, temp, groupRects,1, "trained_classifier_erGrouping.xml", 0.1);
	////cv::text::erGrouping(imgRoiPro, channels, regionsMy, temp, groupRects, 0);
	//for (int i = 0; i < groupRects.size(); i++)
	//{
	//	rectangle(imgRoi, groupRects[i], Scalar(255, 255, 0), 2);
	//}

	//imshow("聚类", imgRoi);
	//waitKey(0);

	//----------------------------CNN--------------------------------------------------------------------
	imgTemp = imgRoiPro.clone();
	string vocabulary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	vector<string> lexicon;
	lexicon.push_back((string)("abb"));
	//lexicon.push_back(string("riser"));
	//lexicon.push_back(string("CHINA"));
	//lexicon.push_back(string("HERE"));
	//lexicon.push_back(string("President"));
	//lexicon.push_back(string("smash"));
	//lexicon.push_back(string("KUALA"));
	//lexicon.push_back(string("Produkt"));
	//lexicon.push_back(string("NINTENDO"));
	lexicon.push_back(string("0"));
	lexicon.push_back(string("1"));
	lexicon.push_back(string("2"));
	lexicon.push_back(string("3"));
	lexicon.push_back(string("4"));
	lexicon.push_back(string("5"));
	lexicon.push_back(string("6"));
	lexicon.push_back(string("7"));
	lexicon.push_back(string("8"));
	lexicon.push_back(string("9"));


	Mat transition_p;
	cv::text::createOCRHMMTransitionsTable(vocabulary, lexicon, transition_p);

	Mat emission_p = Mat::eye(62, 62, CV_64FC1);

	Ptr<cv::text::OCRBeamSearchDecoder> ocr = cv::text::OCRBeamSearchDecoder::create(
		cv::text::loadOCRBeamSearchClassifierCNN("OCRBeamSearch_CNN_model_data.xml.gz"),
		vocabulary, transition_p, emission_p, cv::text::OCR_DECODER_VITERBI, 50);

	string output;
	vector<Rect> boxes;
	vector<string> words;
	vector<float> confidences;
	ocr->run(imgRoiPro, output, &boxes, &words, &confidences, cv::text::OCR_LEVEL_WORD);
	for (int i = 0; i < boxes.size(); i++)
	{
		rectangle(imgTemp, boxes[i], Scalar(0, 0, 255), 1);
	}
	imshow("Rect", imgTemp);
	waitKey(0);
	cout << output << endl;
	//cv::KalmanFilter()

}


void getRoiImg(Mat imgSrc, Mat& imgRoi)
{
	Mat imgTempForContour = imgSrc.clone();
	cvtColor(imgTempForContour, imgTempForContour, COLOR_BGR2GRAY);
	threshold(imgTempForContour, imgTempForContour, 0, 255, THRESH_OTSU);
	vector<vector<Point>> roiContours;
	findContours(imgTempForContour, roiContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	int iContourArea = 0;
	int iLabelContour;
	for (int i = 0; i < roiContours.size(); i++)
	{
		int temp = contourArea(roiContours[i]);
		if (iContourArea < temp)
		{
			iContourArea = temp;
			iLabelContour = i;
		}
	}
	Rect roiRect = boundingRect(roiContours[iLabelContour]);
	imgRoi = imgSrc(roiRect);
}