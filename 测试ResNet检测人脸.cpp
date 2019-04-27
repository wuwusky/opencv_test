#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
using namespace dnn;
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
void main()
{
	string prototxt = "..//faceDetector//deploy.prototxt";
	string caffemodel = "..//faceDetector//res10_300x300_ssd_iter_140000.caffemodel";
	Net net;
	net = readNetFromCaffe(prototxt, caffemodel);

	Mat frame = imread("»À¡≥.jpg");
	Mat frameTemp;
	resize(frame, frameTemp, Size(300, 300));
	Mat frameBlob = blobFromImage(frameTemp, 1.0, Size(), Scalar(100,100,100), false, false);
	net.setInput(frameBlob, "data");

	Mat detections = net.forward("detection_out");
	Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

	//cout << detectionMat << endl;

	float confidenceThreshold = 0.15;
	for (int i=0;i< detectionMat.rows;i++)
	{
		float confidence = detectionMat.at<float>(i, 2);

		if (confidence > confidenceThreshold)
		{
			int objClass = (int)(detectionMat.at<float>(i, 1));

			float xL = detectionMat.at<float>(i, 3) * frame.cols;
			float yL = detectionMat.at<float>(i, 4) * frame.rows;

			float xR = detectionMat.at<float>(i, 5) * frame.cols;
			float yR = detectionMat.at<float>(i, 6) * frame.rows;

			std::cout << "÷√–≈∂»£∫" << confidence << endl;

			Rect faceRect((int)xL, (int)yL, (int)(xR - xL), (int)(yR - yL));
			rectangle(frame, faceRect, Scalar(0, 255, 0), 3);
		}

	}

	if (frame.cols > 1500)
	{
		resize(frame, frame, Size(), 0.5, 0.5);
	}
	imshow("detect", frame);
	waitKey(0);



}