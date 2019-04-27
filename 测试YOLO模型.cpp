#include <opencv2/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
using namespace cv::dnn;
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
const size_t network_width = 416;
const size_t network_height = 416;
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

int main(int argc, char** argv)
{
	String modelConfiguration = "..//YOLO//yolo.cfg";
	String modelBinary = "..//YOLO//yolo-voc.weights";

	dnn::Net net = readNetFromDarknet(modelConfiguration, modelBinary);
	cv::Mat frame = cv::imread("bike.jpg");

	cv::Mat resized;
	cv::resize(frame, resized, cv::Size(network_width, network_height));

	Mat inputBlob = blobFromImage(resized, 1 / 255.F);
													  
	net.setInput(inputBlob, "data");                

	cv::Mat detectionMat = net.forward("detection_out");

	float confidenceThreshold = 0.25;
	for (int i = 0; i < detectionMat.rows; i++)
	{
		const int probability_index = 5;
		const int probability_size = detectionMat.cols - probability_index;
		float *prob_array_ptr = &detectionMat.at<float>(i, probability_index);

		size_t objectClass = std::max_element(prob_array_ptr, prob_array_ptr + probability_size) - prob_array_ptr;
		float confidence = detectionMat.at<float>(i, (int)objectClass + probability_index);

		if (confidence > confidenceThreshold)
		{
			float x = detectionMat.at<float>(i, 0);
			float y = detectionMat.at<float>(i, 1);
			float width = detectionMat.at<float>(i, 2);
			float height = detectionMat.at<float>(i, 3);
			float xLeftBottom = (x - width / 2) * frame.cols;
			float yLeftBottom = (y - height / 2) * frame.rows;
			float xRightTop = (x + width / 2) * frame.cols;
			float yRightTop = (y + height / 2) * frame.rows;

			std::cout << "Class: " << objectClass << std::endl;
			std::cout << "Confidence: " << confidence << std::endl;

			std::cout << " " << xLeftBottom
				<< " " << yLeftBottom
				<< " " << xRightTop
				<< " " << yRightTop << std::endl;

			Rect object((int)xLeftBottom, (int)yLeftBottom,
				(int)(xRightTop - xLeftBottom),
				(int)(yRightTop - yLeftBottom));

			rectangle(frame, object, Scalar(0, 255, 0),3);
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