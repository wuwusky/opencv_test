#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <io.h>
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
Rect myRoi;
Mat imgFrame;
string imgCurrentAddr;
bool flagStart = false;
bool flagEnd = false;
void getFiles(string path, vector<string>& files, vector<string>& fileNames);
//----------------------------------------------------------------------------------
static void myOnMouse(int event, int x, int y, int, void*)
{
	switch (event)
	{
	case EVENT_MOUSEMOVE:
	{
		if (flagStart && !flagEnd)
		{
			Mat temp;
			temp = imgFrame.clone();
			//imgFrame = imread(imgCurrentAddr);
			rectangle(temp, Point(myRoi.x, myRoi.y), Point(x, y), Scalar(0, 0, 255), 2);
			imshow("模板", temp);
		}

		break;
	}
	case EVENT_LBUTTONDOWN:
	{
		flagStart = true;
		myRoi.x = x;
		myRoi.y = y;
		flagEnd = false;

		break;
	}
	case EVENT_LBUTTONUP:
	{
		myRoi.width = abs(x - myRoi.x);
		myRoi.height = abs(y - myRoi.y);
		flagEnd = true;
		break;

	}
	}

}

int main()
{

	char* fileAddr = "..\\油位计检测结果//roiModel";
	vector <string> vecFilesAddr, vecFileNames;

	getFiles(fileAddr, vecFilesAddr, vecFileNames);

	fstream wr("标定信息roi.txt", ios::out);

	for (int i = 0; i < vecFilesAddr.size(); i++)
	{
		namedWindow("模板", 1);
		Mat imgSrc = imread(vecFilesAddr[i]);
		//resize(imgSrc, imgSrc, Size(), 0.5, 0.5);
		imgFrame = imgSrc.clone();


		//imshow("模板", imgSrc);
		setMouseCallback("模板", myOnMouse, NULL);

		imshow("模板", imgFrame);

		waitKey(0);

		string tempName;
		char* nameEnd = strchr((char*)(vecFileNames[i].c_str()), '_');
		char* nameStart = (char*)vecFileNames[i].c_str();
		tempName = vecFileNames[i].substr(0, (size_t)(nameEnd - nameStart));

		wr << tempName << " " << myRoi.x  << " " << myRoi.y  << " " << myRoi.width  << " " << myRoi.height  << endl;
		nameEnd = NULL;
		nameStart = NULL;
	}
	wr.close();
	return 1;
}

void getFiles(string path, vector<string>& files, vector<string>& fileNames)
{
	//文件句柄
	long hFile = 0;
	//文件的信息
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

int getDataLabel(string fileName, string labelStopFlage)//根据文件名得到样本的标签序号
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