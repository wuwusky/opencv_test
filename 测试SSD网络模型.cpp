#include <opencv2/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
using namespace cv::dnn;
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
const size_t width = 300;
const size_t height = 300;
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
static Mat getMean(const size_t& imageHeight, const size_t& imageWidth)
{
	Mat mean;

	const int meanValues[3] = { 104, 117, 123 };
	vector<Mat> meanChannels;
	for (int i = 0; i < 3; i++)
	{
		Mat channel((int)imageHeight, (int)imageWidth, CV_32F, Scalar(meanValues[i]));
		meanChannels.push_back(channel);
	}
	cv::merge(meanChannels, mean);
	return mean;
}

static Mat preprocess(const Mat& frame)
{
	Mat preprocessed;
	frame.convertTo(preprocessed, CV_32F);
	resize(preprocessed, preprocessed, Size(width, height)); //SSD accepts 300x300 RGB-images

	Mat mean = getMean(width, height);
	cv::subtract(preprocessed, mean, preprocessed);

	return preprocessed;
}

int main(int argc, char** argv)
{
	String modelConfiguration = "..//SSD//deploy300.prototxt";
	String modelBinary = "..//SSD//SSD_300.caffemodel";
	dnn::Net net = readNetFromCaffe(modelConfiguration, modelBinary);

	cv::Mat frame = cv::imread("person.jpg", -1);

	//Mat preprocessedFrame = preprocess(frame);
	Mat frameTemp;
	resize(frame, frameTemp, Size(width, height));
	Mat inputBlob = blobFromImage(frameTemp, 1.0f, Size(), Scalar(), false);
	net.setInput(inputBlob, "data");
	Mat detection = net.forward("detection_out");


	Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
	float confidenceThreshold = 0.7;
	for (int i = 0; i < detectionMat.rows; i++)
	{
		float confidence = detectionMat.at<float>(i, 2);

		if (confidence > confidenceThreshold)
		{
			size_t objectClass = (size_t)(detectionMat.at<float>(i, 1));

			float xLeftBottom = detectionMat.at<float>(i, 3) * frame.cols;
			float yLeftBottom = detectionMat.at<float>(i, 4) * frame.rows;
			float xRightTop = detectionMat.at<float>(i, 5) * frame.cols;
			float yRightTop = detectionMat.at<float>(i, 6) * frame.rows;

			std::cout << "类别: " << objectClass << std::endl;
			std::cout << "置信度: " << confidence << std::endl;

			std::cout << " " << xLeftBottom
				<< " " << yLeftBottom
				<< " " << xRightTop
				<< " " << yRightTop << std::endl;

			Rect object((int)xLeftBottom, (int)yLeftBottom,
				(int)(xRightTop - xLeftBottom),
				(int)(yRightTop - yLeftBottom));

			rectangle(frame, object, Scalar(0, 255, 0), 3);
		}
	}

	if (frame.cols > 1500)
	{
		resize(frame, frame, Size(), 0.5, 0.5);
	}
	imshow("detections", frame);
	waitKey();

	return 0;
}
