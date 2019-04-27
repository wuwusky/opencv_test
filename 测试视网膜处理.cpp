#include <iostream>
#include <cstring>
#include "opencv2/opencv.hpp"
#include "opencv2/bioinspired.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	cv::Mat inputFrame;
	inputFrame = cv::imread("g2.jpg", 1); // load image in RGB mode
	resize(inputFrame, inputFrame, Size(320, 240));
	cv::Ptr<cv::bioinspired::Retina> myRetina;

	myRetina = cv::bioinspired::createRetina(inputFrame.size());
	//myRetina->clearBuffers();

	cv::Mat retinaOutput_parvo;
	cv::Mat retinaOutput_magno;

	for (int i = 0; i < 20; i ++)
	{
		myRetina->run(inputFrame);
		myRetina->getParvo(retinaOutput_parvo);
		myRetina->getMagno(retinaOutput_magno);
		cv::imshow("retina input", inputFrame);
		cv::imshow("Retina Parvo", retinaOutput_parvo);
		cv::imshow("Retina Magno", retinaOutput_magno);
		waitKey(0);
	}
	//myRetina->run(inputFrame);
	//myRetina->getParvo(retinaOutput_parvo);
	//myRetina->getMagno(retinaOutput_magno);
	//cv::imshow("retina input", inputFrame);
	//cv::imshow("Retina Parvo", retinaOutput_parvo);
	//cv::imshow("Retina Magno", retinaOutput_magno);
	Mat dstImg;
	addWeighted(inputFrame, 0.5, retinaOutput_parvo, 0.5, 1, dstImg);
	imshow("ºÏ³É",dstImg);

	std::cout<<"Retina demo end"<<std::endl;
	waitKey(0);

	return 0;
}
