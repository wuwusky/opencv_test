#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/trace.hpp>
using namespace cv;
using namespace cv::dnn;

#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

/* Find best class for the blob (i. e. class with maximal probability) */
static void getMaxClass(const Mat &probBlob, int *classId, double *classProb)
{
	Mat probMat = probBlob.reshape(1, 1); //reshape the blob to 1x1000 matrix
	Point classNumber;

	minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
	*classId = classNumber.x;
}

static std::vector<String> readClassNames(const char *filename = "synset_words.txt")
{
	std::vector<String> classNames;

	std::ifstream fp(filename);
	if (!fp.is_open())
	{
		std::cerr << "File with classes labels not found: " << filename << std::endl;
		exit(-1);
	}

	std::string name;
	while (!fp.eof())
	{
		std::getline(fp, name);
		if (name.length())
			classNames.push_back(name.substr(name.find(' ') + 1));
	}

	fp.close();
	return classNames;
}

int main(int argc, char **argv)
{
	CV_TRACE_FUNCTION();

	String modelTxt = "bvlc_googlenet.prototxt";
	String modelBin = "bvlc_googlenet.caffemodel";
	String imageFile = "3.jpg";

	//! [Read and initialize network]
	Net net = dnn::readNetFromCaffe(modelTxt, modelBin);
	//! [Read and initialize network]

	//! [Check that network was read successfully]
	if (net.empty())
	{
		std::cerr << "Can't load network by using the following files: " << std::endl;
		std::cerr << "prototxt:   " << modelTxt << std::endl;
		std::cerr << "caffemodel: " << modelBin << std::endl;
		std::cerr << "bvlc_googlenet.caffemodel can be downloaded here:" << std::endl;
		std::cerr << "http://dl.caffe.berkeleyvision.org/bvlc_googlenet.caffemodel" << std::endl;
		exit(-1);
	}
	//! [Check that network was read successfully]

	//! [Prepare blob]
	Mat img = imread(imageFile);
	if (img.empty())
	{
		std::cerr << "Can't read image from the file: " << imageFile << std::endl;
		exit(-1);
	}

	//GoogLeNet accepts only 224x224 RGB-images
	Mat inputBlob = blobFromImage(img, 1, Size(224, 224),
		Scalar(104, 117, 123));   //Convert Mat to batch of images
								  //! [Prepare blob]

	Mat prob;
	cv::TickMeter t;
	for (int i = 0; i < 10; i++)
	{
		CV_TRACE_REGION("forward");
		//! [Set input blob]
		net.setInput(inputBlob, "data");        //set the network input
												//! [Set input blob]
		t.start();
		//! [Make forward pass]
		prob = net.forward("prob");                          //compute output
															 //! [Make forward pass]
		t.stop();
	}

	//! [Gather output]
	int classId;
	double classProb;
	getMaxClass(prob, &classId, &classProb);//find the best class
											//! [Gather output]

											//! [Print results]
	std::vector<String> classNames = readClassNames();
	std::cout << "Best class: #" << classId << " '" << classNames.at(classId) << "'" << std::endl;
	std::cout << "Probability: " << classProb * 100 << "%" << std::endl;
	//! [Print results]
	std::cout << "Time: " << (double)t.getTimeMilli() / t.getCounter() << " ms (average from " << t.getCounter() << " iterations)" << std::endl;

	return 0;
} //main
