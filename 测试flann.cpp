#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <iostream>
#include <fstream>
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
using namespace std;
using namespace cv;
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
void DoubleMatch(vector<DMatch> MatchVec_1, vector<DMatch> MatchVec_2, vector<DMatch> &finalMatch);
void keyPointResize(vector<KeyPoint>& KeyPoint1, vector<KeyPoint>& KeyPoint2, int keyPointsMaxNum);
//-----------------------------------------------------------------------
int main()
{
	Mat imgTmp, imgDet;
	
	imgTmp = imread("..//testServoData//1.jpg", 0);
	imgDet = imread("..//testServoData//1-1.jpg", 0);

	Ptr<cv::xfeatures2d::SurfFeatureDetector> detectorS = cv::xfeatures2d::SurfFeatureDetector::create(100, 4, 3, false, false);
	Ptr<cv::xfeatures2d::SurfDescriptorExtractor> descriptorS = cv::xfeatures2d::SurfDescriptorExtractor::create(100, 4, 3, false, false);
	vector<KeyPoint> keyPTmp, keyPDet;
	Mat keyDesTmp, keyDesDet;

	double t1 = getTickCount();
	detectorS->detect(imgTmp, keyPTmp);
	descriptorS->compute(imgTmp, keyPTmp, keyDesTmp);
	cout << "模板特征点数：" << keyPTmp.size() << endl;
	detectorS->detect(imgDet, keyPDet);
	descriptorS->compute(imgDet, keyPDet, keyDesDet);
	cout << "测试特征点数：" << keyPDet.size();
	double t2 = getTickCount();
	double t = (t2 - t1) / getTickFrequency();
	cout << "  " << "耗时：" << t << endl;



	t1 = getTickCount();
	vector<KeyPoint> keyPTmp1, keyPDet1;
	Mat keyDesTmp1, keyDesDet1;
	Ptr<cv::xfeatures2d::SURF> detAndDes = cv::xfeatures2d::SURF::create(100, 4, 3, false, false);
	detAndDes->detectAndCompute(imgTmp, Mat(), keyPTmp1, keyDesTmp1);
	detAndDes->detectAndCompute(imgDet, Mat(), keyPDet1, keyDesDet1);
	cout << "模板特征点数：" << keyPTmp1.size() << endl;
	cout << "测试特征点数：" << keyPDet1.size();
	t2 = getTickCount();
	t = (t2 - t1) / getTickFrequency();
	cout << "   " << "耗时：" << t << endl;

	vector<KeyPoint>keyPTemp;
	for (int i = 0; i < keyPTmp.size(); i++)
	{
		keyPTemp.push_back(keyPTmp[i]);
	}

	vector<KeyPoint>keyPTemp1, keyPTemp2;

	keyPointResize(keyPTemp, keyPTemp1, 3000);
	KeyPointsFilter fff;
	fff.runByKeypointSize(keyPTmp, 20, 3000);



	t1 = getTickCount();
	vector<DMatch> matchesBF;
	BFMatcher matcherBF(NORM_L2, false);
	matcherBF.match(keyDesTmp, keyDesDet, matchesBF);
	t2 = getTickCount();
	t = (t2 - t1) / getTickFrequency();
	cout << "matchSize:"<<matchesBF.size() << "   "<<"time:" << t << endl;

	t1 = getTickCount();
	vector<DMatch> matchesFLANN;
	FlannBasedMatcher matcherFLANN;
	matcherFLANN.match(keyDesTmp, keyDesDet, matchesFLANN);
	t2 = getTickCount();
	t = (t2 - t1) / getTickFrequency();
	cout << "matchSize:" << matchesFLANN.size() << "   " << "time:" << t << endl;


	double t11 = getTickCount();
	vector<DMatch> matchesPR, matchesPRInv;
	vector<DMatch> good_matches;
	BFMatcher matcherPR(NORM_L2, false);

	matcherPR.match(keyDesTmp, keyDesDet, matchesPR);
	matcherPR.match(keyDesDet, keyDesTmp, matchesPRInv);
	DoubleMatch(matchesPR, matchesPRInv, good_matches);
	double t22 = getTickCount();
	double tt = (t22 - t11) / getTickFrequency();
	cout << "正配对：" << matchesPR.size() << "   "<<"反配对：" << matchesPRInv.size() << endl;
	cout << "matchSize:" << good_matches.size() << "   " << "timePRBF:" << tt << endl;


	t11 = getTickCount();
	vector<DMatch> matchesPRFlann, matchesPRInvFlann;
	vector<DMatch> good_matchesF;
	FlannBasedMatcher matcherFLANNPR;

	matcherFLANNPR.match(keyDesTmp, keyDesDet, matchesPRFlann);
	matcherFLANNPR.match(keyDesDet, keyDesTmp, matchesPRInvFlann);
	DoubleMatch(matchesPRFlann, matchesPRInvFlann, good_matchesF);
	t22 = getTickCount();
	tt = (t22 - t11) / getTickFrequency();
	cout << "正配对：" << matchesPRFlann.size() << "   " << "反配对：" << matchesPRInvFlann.size() << endl;
	cout << "matchSize:" << good_matchesF.size() << "   " << "timePRFLANN:" << tt << endl;




	//fstream wr1("match1.txt", ios::out);
	//fstream wr2("match2.txt", ios::out);
	//fstream wr1_1("match1_1.txt", ios::out);
	//fstream wr2_1("match2_1.txt", ios::out);
	//fstream wr3("match3.txt", ios::out);
	//fstream wr3_1("match3_1.txt", ios::out);
	//for (int i = 0; i < matchesPR.size(); i++)
	//{
	//	wr1 << matchesPR[i].queryIdx << " " << matchesPR[i].trainIdx << " " << matchesPR[i].distance << endl;
	//	wr1_1 << matchesPRFlann[i].queryIdx << " " << matchesPRFlann[i].trainIdx << " " << matchesPRFlann[i].distance << endl;
	//}





	return 0;
}

//双向匹配
void DoubleMatch(vector<DMatch> MatchVec_1, vector<DMatch> MatchVec_2, vector<DMatch> &finalMatch)
{
	int l = MatchVec_1.size();
	int r = MatchVec_2.size();

	for (int i = 0; i < l; i++)
	{
		int Qeuryone = MatchVec_1[i].queryIdx;
		int Trainone = MatchVec_1[i].trainIdx;
		for (int j = 0; j < r; j++)
		{
			int Qeurytwo = MatchVec_2[j].queryIdx;
			int Traintwo = MatchVec_2[j].trainIdx;
			if (Qeuryone == Traintwo && Qeurytwo == Trainone)
			{
				finalMatch.push_back(MatchVec_1[i]);
				break;
			}
		}
	}
}

//调整特征点数量，间隔抽样
void keyPointResize(vector<KeyPoint>& KeyPoint1, vector<KeyPoint>& KeyPoint2, int keyPointsMaxNum)
{
	if (KeyPoint1.size() <= 0)
	{
		//return NULL;
	}
	else if (KeyPoint1.size() > keyPointsMaxNum)
	{
		if (KeyPoint1.size() < 2 * keyPointsMaxNum)
		{
			KeyPoint2.resize(keyPointsMaxNum);

			memcpy(&KeyPoint2[0], &KeyPoint1[0],
				keyPointsMaxNum * sizeof(KeyPoint));
		}
		else
		{
			int iS = KeyPoint1.size() / keyPointsMaxNum;

			for (int i = 0; i < KeyPoint1.size(); i += iS)
			{
				KeyPoint2.push_back(KeyPoint1[i]);
			}
		}
	}
	else
	{
		KeyPoint2.resize(KeyPoint1.size());

		memcpy(&KeyPoint2[0], &KeyPoint1[0],
			KeyPoint1.size() * sizeof(KeyPoint));
	}

}