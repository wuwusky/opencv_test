#include "opencv2\opencv.hpp"
#include <iostream>
#include <io.h>
#include <vector>
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
void main()
{
	string filePath = "D:\\opencv3.1\\extra\\opencv_extra-master\\testdata\\cv\\structured_light\\data\\";
	string filesPathTemp = filePath + "*.jpg";
	const char* filesPath = filesPathTemp.c_str();
	struct _finddata_t fileInfo;
	FileStorage fs("images.yml", FileStorage::WRITE);
	//FileStorage fs("images.yml", FileStorage::READ);

	//fs<< "cam1" <<"cam2:";

	long handle = _findfirst(filesPath, &fileInfo);
	if (handle == -1L)
	{
		cout << "没找到文件"<<endl;
	}

	do 
	{
		if (strcmp(fileInfo.name, ".") != 0 &&strcmp(fileInfo.name, "..") != 0)
		{
			string fileCurrentPath = filePath + fileInfo.name;
			const char* temp = fileCurrentPath.c_str();
			fs<<"str"<<temp;
		}
	} while (_findnext(handle, &fileInfo) == 0);

	_findclose(handle);
}