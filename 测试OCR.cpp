#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc/edge_filter.hpp>
#include <opencv2/text.hpp>
#include <opencv2/ximgproc.hpp>
//-------------------------------------------------------------//
//-------------------------------------------------------------//
using namespace cv;
using namespace std;
using namespace ximgproc;
using namespace cv::text;
//-------------------------------------------------------------//
//-------------------------------------------------------------//
void er_draw(vector<Mat> &channels, vector<vector<ERStat> > &regions, vector<Vec2i> group, Mat& segmentation); 
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

	boxFilter(src, mean, CV_64FC1, Size(blockSize, blockSize), Point(-1, -1), true, BORDER_REPLICATE);
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
//-------------------------------------------------------------//
//-------------------------------------------------------------//
int main()
{
	Mat Image = imread("4.jpg",1);
	//if (Image.channels() > 1)
	//{
	//	cvtColor(Image, Image, COLOR_BGR2GRAY);
	//}
	//myNiBlackThreshold(Image, Image, 255, THRESH_BINARY, 75 , 1);
	////threshold(Image, Image, 0, 255, THRESH_OTSU);
	//imshow("1", Image);
	////waitKey(0);
	//cvtColor(Image, Image, COLOR_GRAY2BGR);

	if (Image.cols > 1000)
	{
		resize(Image, Image, Size(800, 500));
	}
	//*******************文字分割部分************************
	vector <Mat> channels;

	Mat grey;
	cvtColor(Image, grey, COLOR_RGB2GRAY);

	channels.push_back(grey);
	channels.push_back(255 - grey);

	Ptr<ERFilter> er_filter1 = createERFilterNM1(loadClassifierNM1("trained_classifierNM1.xml"), 8, 0.00015f, 0.13f, 0.2f, true, 0.1f);
	Ptr<ERFilter> er_filter2 = createERFilterNM2(loadClassifierNM2("trained_classifierNM2.xml"), 0.5);

	vector<vector<ERStat>> regions(channels.size());

	for (int c = 0; c < (int)channels.size(); c++)
	{
		er_filter1->run(channels[c], regions[c]);
		er_filter2->run(channels[c], regions[c]);
	}
	
	Mat out_img_decomposition = Mat::zeros(Image.rows+2, Image.cols+2, CV_8UC1);
	vector<Vec2i> tmp_group;

	//for (int i = 0; i<(int)regions.size(); i++)
	//{
	//	for (int j = 0;j<(int)regions[i].size();j++)
	//	{

	//	}
	//}

	vector<vector<Vec2i>> nm_region_groups;
	vector<Rect> nm_boxes;
	erGrouping(Image, channels, regions, nm_region_groups, nm_boxes, ERGROUPING_ORIENTATION_ANY, "trained_classifier_erGrouping.xml");

	Mat temp = Image.clone();

	for (int i = 0; i < regions.size(); i++)
	{
		for (int j = 0; j < regions[i].size(); j++)
		{
			rectangle(temp, regions[i][j].rect, Scalar(0,0,255), 1);
		}

	}

	imshow("2",temp);

	//*******************文字识别部分************************
	Ptr <OCRTesseract> ocr = OCRTesseract::create();
	string output;

	Mat out_img;
	Mat out_img_detection;
	Mat out_img_segmentation = Mat::zeros(Image.rows+2, Image.cols+2, CV_8UC1);
	Image.copyTo(out_img);
	Image.copyTo(out_img_detection);
	float scale_img = 600.f/Image.rows;
	float scale_font = (float)(2-scale_img)/1.4f;
	vector<string> words_detection;

	for (int i = 0; i<(int)nm_boxes.size(); i++)
	{
		rectangle(out_img_detection, nm_boxes[i].tl(),nm_boxes[i].br(), Scalar(0,255,255), 3);
		imshow("3",out_img_detection);

		Mat group_img = Mat::zeros(Image.rows+2, Image.cols+2,CV_8UC1);
		er_draw(channels, regions, nm_region_groups[i], group_img);
		group_img(nm_boxes[i]).copyTo(group_img);
		copyMakeBorder(group_img, group_img, 15, 15, 15, 15, BORDER_CONSTANT, Scalar(0));

		vector<Rect> boxes;
		vector<string> words;
		vector<float> confidences;
		ocr->run(group_img, output, &boxes, &words, &confidences, OCR_LEVEL_WORD);


	}










	waitKey(0);

	return 1;
}  
//-----------------------------------------------------------------------------------------------------------------------
void er_draw(vector<Mat> &channels, vector<vector<ERStat> > &regions, vector<Vec2i> group, Mat& segmentation) 
{ 
    for (int r=0; r<(int)group.size(); r++) 
    { 
		ERStat er = regions[group[r][0]][group[r][1]]; 
		if (er.parent != NULL) // deprecate the root region 
		{ 
			int newMaskVal = 255; 
			int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY; 
			floodFill(channels[group[r][0]],segmentation,Point(er.pixel%channels[group[r][0]].cols,er.pixel/channels[group[r][0]].cols), 
			Scalar(255),0,Scalar(er.level),Scalar(0),flags); 
		} 
	} 
} 
