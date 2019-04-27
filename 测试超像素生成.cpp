#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
void CopyImgFromMask(Mat& src, Mat mask);

int main()
{
	Mat imgSrc;
	imgSrc = cv::imread("..\\roi\\00002_0030.jpg", 1);
	imshow("src", imgSrc);
	//resize(imgSrc, imgSrc, Size(50, 200));
	Mat maskLSC, maskSEEDS, maskSLIC;
	Mat imgDstLSC, imgDstSEEDS, imgDstSLIC;
	imgDstLSC = imgSrc.clone();
	imgDstSEEDS = imgSrc.clone();
	imgDstSLIC = imgSrc.clone();
	Mat labelLSC, labelSEEDS, labelSLIC;

	//--------------------------------------------------LSC-----------------------------------------------------
	Ptr<ximgproc::SuperpixelLSC>  LSC;
	LSC = ximgproc::createSuperpixelLSC(imgSrc);
	LSC->iterate(20);
	LSC->enforceLabelConnectivity(25);
	LSC->getLabels(labelLSC);
	LSC->getLabelContourMask(maskLSC);
	int numLSC = LSC->getNumberOfSuperpixels();
	
	cout << "LSC:" << numLSC << endl;
	Mat temp = Mat::zeros(imgSrc.size(), CV_8UC3);
	//maskLSC.convertTo(maskLSC, CV_8UC1, 1, 0);
	//cout << maskLSC << endl;
	CopyImgFromMask(imgDstLSC, maskLSC);
	CopyImgFromMask(temp, maskLSC);
	//cout << labelLSC << endl;

	imshow("LSC", imgDstLSC);
	imshow("LSCMASK", temp);
	waitKey(0);
	//--------------------------------------------------SEEDS-----------------------------------------------------
	Ptr<ximgproc::SuperpixelSEEDS> SEEDS;
	SEEDS = ximgproc::createSuperpixelSEEDS(imgSrc.cols, imgSrc.rows, imgSrc.channels(), 1000, 20);
	SEEDS->iterate(imgSrc, 4);
	SEEDS->getLabels(labelSEEDS);
	SEEDS->getLabelContourMask(maskSEEDS);
	int numSeeds = SEEDS->getNumberOfSuperpixels();
	//cout << maskSEEDS << endl;
	cout << "SEEDS:"<< numSeeds << endl;
	//imshow("SEEDS", imgDstSEEDS);
	//waitKey(0);
	temp = Mat::zeros(imgSrc.size(), CV_8UC3);
	CopyImgFromMask(imgDstSEEDS, maskSEEDS);
	CopyImgFromMask(temp, maskSEEDS);

	imshow("SEEDS", imgDstSEEDS);
	imshow("SEEDSMASK", temp);
	waitKey(0);
	//--------------------------------------------------SLIC-----------------------------------------------------
	Ptr<ximgproc::SuperpixelSLIC> SLIC;
	SLIC = ximgproc::createSuperpixelSLIC(imgSrc, 100, 10, 10);
	SLIC->iterate(10);
	SLIC->enforceLabelConnectivity();
	SLIC->getLabels(labelSLIC);
	SLIC->getLabelContourMask(maskSLIC);
	int numSLIC = SLIC->getNumberOfSuperpixels();
	cout << "SLIC:" << numSLIC << endl;
	temp = Mat::zeros(imgSrc.size(), CV_8UC3);
	CopyImgFromMask(imgDstSLIC, maskSLIC);
	CopyImgFromMask(temp, maskSLIC);

	imshow("SLIC", imgDstSLIC);
	imshow("SLICMASK", temp);
	waitKey(0);


	//vector<cv::Mat> Mchannels;
	//split(imgSrc, Mchannels);

	//imshow("B", Mchannels[0]);
	//imshow("G", Mchannels[1]);
	//imshow("R", Mchannels[2]);
	//waitKey(0);
	//cv::Mat imgSrcHsv;
	//cvtColor(imgSrc, imgSrcHsv, COLOR_BGR2HSV_FULL);
	//split(imgSrcHsv, Mchannels);

	//imshow("H", Mchannels[0]);
	//imshow("S", Mchannels[1]);
	//imshow("V", Mchannels[2]);
	//waitKey(0);


}

void CopyImgFromMask(Mat& src, Mat mask)
{
	for (int i = 0; i < mask.rows; i++)
	{
		for (int j = 0; j < mask.cols; j++)
		{
			int aa = mask.at<uchar>(i, j);
			if (aa == 255)
			{
				src.at<cv::Vec3b>(i, j) = cv::Vec3b(255, 255, 255);
			}
		}
	}

}