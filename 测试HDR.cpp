#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

void loadExposureSeq(String, vector<Mat>&, vector<float>&);

int main(int, char**argv)
{
	vector<Mat> images;
	vector<float> times;
	loadExposureSeq("D:\\computer visio\\测试导向滤波\\测试HDR", images, times);

	Mat response;
	//Ptr<CalibrateDebevec> calibrate = createCalibrateDebevec();//获取相机反馈函数
	//calibrate->process(images, response, times);

	Mat hdr;
	Ptr<MergeDebevec> merge_debevec = createMergeDebevec();//合成HDR图像
	merge_debevec->process(images, hdr, times, response);

	Mat ldr;
	Ptr<TonemapDurand> tonemap = createTonemapDurand(2.2f);//利用色调映射生成LDR图像
	tonemap->process(hdr, ldr);

	Mat fusion;
	Ptr<MergeMertens> merge_mertens = createMergeMertens(); //里面三个参数分别对应(对比度，饱和度，曝光度)
	merge_mertens->process(images, fusion);

	imwrite("fusion.png", fusion*255 );
	imwrite("ldr.png", ldr*255);
	hdr.convertTo(hdr, CV_8UC3, 1, 50);
	imwrite("hdr.hdr", hdr);

	Mat temp;
	resize(hdr, temp, Size(500, 300));
	imshow("hdr", temp);
	resize(ldr, temp, Size(500, 300));
	imshow("LDR", temp);
	resize(fusion, temp, Size(500, 300));
	imshow("fusion", temp);
	waitKey(0);
	return 0;
}

void loadExposureSeq(String path, vector<Mat>& images, vector<float>& times)
{
	path = path + std::string("/");
	ifstream list_file((path + "test.txt").c_str());
	string name;
	float val;
	while(list_file >> name >> val) {
		Mat img = imread(path + name);
		images.push_back(img);
		times.push_back(1/val);
	}
	list_file.close();
}