#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/trace.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/shape_utils.hpp>
#include <io.h>
#include <fstream>
#include <iostream>
#include <cstdlib>
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
using namespace cv;
using namespace cv::dnn;
using namespace std;
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

Mat getData(Mat img);
void getFiles(string path, vector<string>&files, vector<string>& fileNames);
int getDataLabel(string fileName, string labelStopFlage);
Mat getImgPre(Mat imgSrc);
Mat getImgPre2(Mat imgSrc);
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------

/* Find best class for the blob (i. e. class with maximal probability) */
static void getMaxClass(const Mat &probBlob, int *classId, double *classProb)
{
	Mat probMat = probBlob.reshape(1, 1); //reshape the blob to 1x1000 matrix
	Point classNumber;

	minMaxLoc(probMat, NULL, classProb, NULL, &classNumber);
	*classId = classNumber.x;
}
//----------------------------------------------------------------------------------------------------------

static std::vector<String> readClassNames(const char *filename = "..//alexNet//label.txt")
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
//----------------------------------------------------------------------------------------------------------

Mat getData(Mat img)
{
	int num_channels_ = img.channels();
	Mat sample_float;
	img.convertTo(sample_float, CV_32F);

	double dataMean = 0;
	cv::Vec3d dataMeanV = cv::Vec3b(0, 0, 0);
	if (num_channels_ == 3)
	{
		for (int n = 0; n < sample_float.channels(); n++)
		{
			for (int i = 0; i < sample_float.rows; i++)
			{
				for (int j = 0; j < sample_float.cols; j++)
				{
					dataMeanV[n] = dataMeanV[n] + (double)sample_float.at<cv::Vec3f>(i, j)[n];
				}
			}
		}

	}
	else
	{
		for (int n = 0; n < sample_float.channels(); n++)
		{
			for (int i = 0; i < sample_float.rows; i++)
			{
				for (int j = 0; j < sample_float.cols; j++)
				{
					dataMean = dataMean + (double)sample_float.at<float>(i, j);
				}
			}
		}
	}
	dataMean = dataMean / (sample_float.rows * sample_float.cols);
	for (int i = 0; i < num_channels_; i++)
	{
		dataMeanV[i] = dataMeanV[i] / (sample_float.rows*sample_float.cols);
	}
	if (num_channels_ == 3)
	{
		for (int n = 0; n < sample_float.channels(); n++)
		{
			for (int i = 0; i < sample_float.rows; i++)
			{
				for (int j = 0; j < sample_float.cols; j++)
				{
					sample_float.at<cv::Vec3f>(i, j)[n] = sample_float.at<cv::Vec3f>(i, j)[n] - dataMeanV[n];
				}
			}
		}

	}
	else
	{
		for (int n = 0; n < sample_float.channels(); n++)
		{
			for (int i = 0; i < sample_float.rows; i++)
			{
				for (int j = 0; j < sample_float.cols; j++)
				{
					sample_float.at<float>(i, j) = sample_float.at<float>(i, j) - dataMean;
				}
			}
		}
	}
	Mat sample_normalized;
	sample_normalized = sample_float.clone();

	return sample_normalized;
}
//----------------------------------------------------------------------------------------------------------

int test1()
{
	CV_TRACE_FUNCTION();

	String modelTxt = "..//googleNet//deploy.prototxt";
	String modelBin = "..//googleNet//bvlc_googlenet_iter_700.caffemodel";
	String imageFile = "D:\\ML\\压板roi\\yabanTest\\1_1 (141).jpg";

	Net net = dnn::readNetFromCaffe(modelTxt, modelBin);

	Mat img = imread(imageFile, 1);
	Mat inputBlob = getData(img);
	Mat inputBlobNew = blobFromImage(inputBlob, 1.0, Size(227, 227), Scalar(), false);
	Mat prob;
	net.setInput(inputBlobNew, "data");        //set the network input

	prob = net.forward("prob");                          //compute output
														 //! [Gather output]
	int classId;
	double classProb;
	cout << prob << endl;
	getMaxClass(prob, &classId, &classProb);//find the best class

	std::vector<String> classNames = readClassNames();
	std::cout << "Best class: #" << classId << " '" << classNames.at(classId) << "'" << std::endl;
	std::cout << "Probability: " << classProb * 100 << "%" << std::endl;
	//! [Print results]
	cv::imshow("test", img);
	waitKey(0);
	destroyAllWindows();
	return 0;
} //main
  //----------------------------------------------------------------------------------------------------------

int testall()
{
	String modelTxt = "..//alexNet//deployRGB.prototxt";
	String modelBin = "..//alexNet//caffe_alexnet_train_RGB_iter_1000.caffemodel";

	vector<string> vecFiles, vecFileNames;
	string testPath = "D:\\ML\\压板roi\\yabanTest";
	getFiles(testPath, vecFiles, vecFileNames);

	Net net = dnn::readNetFromCaffe(modelTxt, modelBin);
	int iNumRight = 0;

	for (int i=0;i< vecFiles.size();i++)
	{
		Mat img = imread(vecFiles[i], 1);
		Mat imgG;
		//cvtColor(img, imgG, COLOR_BGR2GRAY);
		//imgG = getImgPre(imgG);
		Mat imgEn = getImgPre2(img);
		Mat inputBlob = getData(img);
		Mat inputBlobNew = blobFromImage(inputBlob, 1.0, Size(100, 200), Scalar(), false);
		Mat prob;
		net.setInput(inputBlobNew, "data");        //set the network input

		prob = net.forward("prob");                          //compute output
		int classId;
		double classProb;
		getMaxClass(prob, &classId, &classProb);//find the best class

		int iLabel = getDataLabel(vecFileNames[i], "_");

		if (iLabel == classId)
		{
			iNumRight++;
			imwrite("..//alexNet//分类结果//正确分类//" + vecFileNames[i], img);
		}
		else
		{
			imwrite("..//alexNet//分类结果//错误分类//" + vecFileNames[i], img);
			cout << prob << endl;
		}

	}
	double dRatioRight = (double)iNumRight / (double)vecFiles.size();
	cout << "正确率：" << dRatioRight << endl;

	return 1;
}
//----------------------------------------------------------------------------------------------------------

void getFiles(string path, vector<string>&files, vector<string>& fileNames)
{
	__int64 hFile = 0;
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录，就进行迭代
			//如果不是目录，就加入文件列表files
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files, fileNames);
				}
			}
			else
			{
				fileNames.push_back(fileinfo.name);
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}
//----------------------------------------------------------------------------------------------------------
int getDataLabel(string fileName, string labelStopFlage)
{
	string file = fileName;
	string label[20];

	for (int i = 0; i < file.size(); i++)
	{
		if (file.substr(i, 1).compare(labelStopFlage))
		{
			label[i] = file.substr(i, 1);
		}
		else
		{
			break;
		}
	}

	std::stringstream temp;
	int labelTrainData;

	temp << label[0] + label[1];
	temp >> labelTrainData;

	return labelTrainData;
}
//----------------------------------------------------------------------------------------------------------
Mat getImgPre2(Mat imgSrc)
{
	Mat imgDst;
	vector<Mat> vecChanels;
	split(imgSrc, vecChanels);
	for (int i = 0; i < vecChanels.size(); i++)
	{
		equalizeHist(vecChanels[i], vecChanels[i]);
		Mat temp = vecChanels[i].clone();
		bilateralFilter(temp, vecChanels[i], 15, 100, 100);
	}
	merge(vecChanels, imgDst);
	return imgDst;
}

Mat getImgPre(Mat imgSrc)
{
	Mat imgGray;
	if (imgSrc.channels() > 1)
	{
		cvtColor(imgSrc, imgGray, COLOR_BGR2GRAY);
	}
	else
	{
		imgGray = imgSrc.clone();
	}
	Mat imgDst;
	equalizeHist(imgGray, imgDst);
	return imgDst;
}
//----------------------------------------------------------------------------------------------------------

void main()
{
	//test1();

	testall();
}