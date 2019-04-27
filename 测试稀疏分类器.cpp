// 稀疏表示分类器测试.cpp : 定义控制台应用程序的入口点。
//
#include "opencv2/opencv.hpp"
#include<KL1p.h>
#include<KLab.h>
#include<KL1pInclude.h>
#include<KLabInclude.h>
#include<KSciInclude.h>
using namespace std;
using namespace cv;
using namespace kl1p;


int main()
{
	// FaceRecognize.cpp : 定义控制台应用程序的入口点。

	cv::Mat imageArray[150], image, image1;
	imageArray[0] = imread("database\\001\\01.jpg");//001
	imageArray[1] = imread("database\\001\\02.jpg");
	imageArray[2] = imread("database\\001\\03.jpg");
	imageArray[3] = imread("database\\001\\04.jpg");
	imageArray[4] = imread("database\\001\\05.jpg");
	imageArray[5] = imread("database\\001\\06.jpg");
	imageArray[6] = imread("database\\001\\07.jpg");
	imageArray[7] = imread("database\\001\\08.jpg");
	imageArray[8] = imread("database\\001\\09.jpg");
	imageArray[9] = imread("database\\001\\10.jpg");

	imageArray[10] = imread("database\\002\\01.jpg");//002
	imageArray[11] = imread("database\\002\\02.jpg");
	imageArray[12] = imread("database\\002\\03.jpg");
	imageArray[13] = imread("database\\002\\04.jpg");
	imageArray[14] = imread("database\\002\\05.jpg");
	imageArray[15] = imread("database\\002\\06.jpg");
	imageArray[16] = imread("database\\002\\07.jpg");
	imageArray[17] = imread("database\\002\\08.jpg");
	imageArray[18] = imread("database\\002\\09.jpg");
	imageArray[19] = imread("database\\002\\10.jpg");

	imageArray[20] = imread("database\\003\\01.jpg");//003
	imageArray[21] = imread("database\\003\\02.jpg");
	imageArray[22] = imread("database\\003\\03.jpg");
	imageArray[23] = imread("database\\003\\04.jpg");
	imageArray[24] = imread("database\\003\\05.jpg");
	imageArray[25] = imread("database\\003\\06.jpg");
	imageArray[26] = imread("database\\003\\07.jpg");
	imageArray[27] = imread("database\\003\\08.jpg");
	imageArray[28] = imread("database\\003\\09.jpg");
	imageArray[29] = imread("database\\003\\10.jpg");

	imageArray[30] = imread("database\\004\\01.jpg");//004
	imageArray[31] = imread("database\\004\\02.jpg");
	imageArray[32] = imread("database\\004\\03.jpg");
	imageArray[33] = imread("database\\004\\04.jpg");
	imageArray[34] = imread("database\\004\\05.jpg");
	imageArray[35] = imread("database\\004\\06.jpg");
	imageArray[36] = imread("database\\004\\07.jpg");
	imageArray[37] = imread("database\\004\\08.jpg");
	imageArray[38] = imread("database\\004\\09.jpg");
	imageArray[39] = imread("database\\004\\10.jpg");

	imageArray[40] = imread("database\\005\\01.jpg");//005
	imageArray[41] = imread("database\\005\\02.jpg");
	imageArray[42] = imread("database\\005\\03.jpg");
	imageArray[43] = imread("database\\005\\04.jpg");
	imageArray[44] = imread("database\\005\\05.jpg");
	imageArray[45] = imread("database\\005\\06.jpg");
	imageArray[46] = imread("database\\005\\07.jpg");
	imageArray[47] = imread("database\\005\\08.jpg");
	imageArray[48] = imread("database\\005\\09.jpg");
	imageArray[49] = imread("database\\005\\10.jpg");

	imageArray[50] = imread("database\\006\\01.jpg");//006
	imageArray[51] = imread("database\\006\\02.jpg");
	imageArray[52] = imread("database\\006\\03.jpg");
	imageArray[53] = imread("database\\006\\04.jpg");
	imageArray[54] = imread("database\\006\\05.jpg");
	imageArray[55] = imread("database\\006\\06.jpg");
	imageArray[56] = imread("database\\006\\07.jpg");
	imageArray[57] = imread("database\\006\\08.jpg");
	imageArray[58] = imread("database\\006\\09.jpg");
	imageArray[59] = imread("database\\006\\10.jpg");

	imageArray[60] = imread("database\\007\\01.jpg");//007
	imageArray[61] = imread("database\\007\\02.jpg");
	imageArray[62] = imread("database\\007\\03.jpg");
	imageArray[63] = imread("database\\007\\04.jpg");
	imageArray[64] = imread("database\\007\\05.jpg");
	imageArray[65] = imread("database\\007\\06.jpg");
	imageArray[66] = imread("database\\007\\07.jpg");
	imageArray[67] = imread("database\\007\\08.jpg");
	imageArray[68] = imread("database\\007\\09.jpg");
	imageArray[69] = imread("database\\007\\10.jpg");


	imageArray[70] = imread("database\\008\\01.jpg");//008
	imageArray[71] = imread("database\\008\\02.jpg");
	imageArray[72] = imread("database\\008\\03.jpg");
	imageArray[73] = imread("database\\008\\04.jpg");
	imageArray[74] = imread("database\\008\\05.jpg");
	imageArray[75] = imread("database\\008\\06.jpg");
	imageArray[76] = imread("database\\008\\07.jpg");
	imageArray[77] = imread("database\\008\\08.jpg");
	imageArray[78] = imread("database\\008\\09.jpg");
	imageArray[79] = imread("database\\008\\10.jpg");

	imageArray[80] = imread("database\\009\\01.jpg");//009
	imageArray[81] = imread("database\\009\\02.jpg");
	imageArray[82] = imread("database\\009\\03.jpg");
	imageArray[83] = imread("database\\009\\04.jpg");
	imageArray[84] = imread("database\\009\\05.jpg");
	imageArray[85] = imread("database\\009\\06.jpg");
	imageArray[86] = imread("database\\009\\07.jpg");
	imageArray[87] = imread("database\\009\\08.jpg");
	imageArray[88] = imread("database\\009\\09.jpg");
	imageArray[89] = imread("database\\009\\10.jpg");

	imageArray[90] = imread("database\\010\\01.jpg");//010
	imageArray[91] = imread("database\\010\\02.jpg");
	imageArray[92] = imread("database\\010\\03.jpg");
	imageArray[93] = imread("database\\010\\04.jpg");
	imageArray[94] = imread("database\\010\\05.jpg");
	imageArray[95] = imread("database\\010\\06.jpg");
	imageArray[96] = imread("database\\010\\07.jpg");
	imageArray[97] = imread("database\\010\\08.jpg");
	imageArray[98] = imread("database\\010\\09.jpg");
	imageArray[99] = imread("database\\010\\10.jpg");

	imageArray[100] = imread("database\\011\\01.jpg");//011
	imageArray[101] = imread("database\\011\\02.jpg");
	imageArray[102] = imread("database\\011\\03.jpg");
	imageArray[103] = imread("database\\011\\04.jpg");
	imageArray[104] = imread("database\\011\\05.jpg");
	imageArray[105] = imread("database\\011\\06.jpg");
	imageArray[106] = imread("database\\011\\07.jpg");
	imageArray[107] = imread("database\\011\\08.jpg");
	imageArray[108] = imread("database\\011\\09.jpg");
	imageArray[109] = imread("database\\011\\10.jpg");

	imageArray[110] = imread("database\\012\\01.jpg");//012
	imageArray[111] = imread("database\\012\\02.jpg");
	imageArray[112] = imread("database\\012\\03.jpg");
	imageArray[113] = imread("database\\012\\04.jpg");
	imageArray[114] = imread("database\\012\\05.jpg");
	imageArray[115] = imread("database\\012\\06.jpg");
	imageArray[116] = imread("database\\012\\07.jpg");
	imageArray[117] = imread("database\\012\\08.jpg");
	imageArray[118] = imread("database\\012\\09.jpg");
	imageArray[119] = imread("database\\012\\10.jpg");

	imageArray[120] = imread("database\\013\\01.jpg");//013
	imageArray[121] = imread("database\\013\\02.jpg");
	imageArray[122] = imread("database\\013\\03.jpg");
	imageArray[123] = imread("database\\013\\04.jpg");
	imageArray[124] = imread("database\\013\\05.jpg");
	imageArray[125] = imread("database\\013\\06.jpg");
	imageArray[126] = imread("database\\013\\07.jpg");
	imageArray[127] = imread("database\\013\\08.jpg");
	imageArray[128] = imread("database\\013\\09.jpg");
	imageArray[129] = imread("database\\013\\10.jpg");

	imageArray[130] = imread("database\\014\\01.jpg");//014
	imageArray[131] = imread("database\\014\\02.jpg");
	imageArray[132] = imread("database\\014\\03.jpg");
	imageArray[133] = imread("database\\014\\04.jpg");
	imageArray[134] = imread("database\\014\\05.jpg");
	imageArray[135] = imread("database\\014\\06.jpg");
	imageArray[136] = imread("database\\014\\07.jpg");
	imageArray[137] = imread("database\\014\\08.jpg");
	imageArray[138] = imread("database\\014\\09.jpg");
	imageArray[139] = imread("database\\014\\10.jpg");

	imageArray[140] = imread("database\\015\\01.jpg");//015
	imageArray[141] = imread("database\\015\\02.jpg");
	imageArray[142] = imread("database\\015\\03.jpg");
	imageArray[143] = imread("database\\015\\04.jpg");
	imageArray[144] = imread("database\\015\\05.jpg");
	imageArray[145] = imread("database\\015\\06.jpg");
	imageArray[146] = imread("database\\015\\07.jpg");
	imageArray[147] = imread("database\\015\\08.jpg");
	imageArray[148] = imread("database\\015\\09.jpg");
	imageArray[149] = imread("database\\015\\10.jpg");

	int i, j, k, g;
	double sum;

	arma::Mat<klab::DoubleReal> A(20139, 150);//共有150张样本图像，有150列，每一个列向量都为一个样本图像
	arma::Mat<klab::DoubleReal> A1(500, 150);
	arma::Col<klab::DoubleReal> Y(20139);
	arma::Col<klab::DoubleReal> Y1(500);
	arma::Col<klab::DoubleReal> W(650);
	arma::Mat<klab::DoubleReal> R(500, 20139);
	arma::Mat<klab::DoubleReal> B(500, 650);
	arma::Col<klab::DoubleReal> x1(150);
	arma::Col<klab::DoubleReal> e1(500);


	fstream f("R.txt",ios::in);
	//fstream f("R.txt");
	for (i = 0; i<500; i++)
	{
		for (j = 0; j<20139; j++)
			f >> R(i, j);
	}
	f.close();

	for (g = 0; g<150; g++)              //赋值B矩阵
	{
		image = imageArray[g];
		if (image.channels() == 3)
		{
			//若是多通道彩色图，则把图片转换为单通道灰色图
			cvtColor(image, image1, CV_BGR2GRAY);
			if (image1.channels() == 1)
				image1.copyTo(image);
		}

		sum = 0;
		for (i = 0; i<image.cols; i++) {
			for (j = 0; j<image.rows; j++) {
				sum = sum + image.at<uchar>(j, i)*image.at<uchar>(j, i);
			}
		}
		sum = sqrt(sum);

		k = 0;
		for (i = 0; i<image.cols; i++) {
			for (j = 0; j<image.rows; j++) {
				A(k++, g) = image.at<uchar>(j, i) / sum;
			}
		}
	}

	A1 = R*A;//A1(500,150)  R(500,20139)  A(20139,150)

	for (i = 0; i<150; i++)
	{
		sum = 0;
		for (j = 0; j<500; j++)
		{
			sum = sum + A1(j, i)*A1(j, i);
		}
		sum = sqrt(sum);
		for (j = 0; j<500; j++)
		{
			A1(j, i) = A1(j, i) / sum;
		}
	}

	for (i = 0; i<500; i++)
	{
		for (j = 0; j<150; j++)
			B(i, j) = A1(i, j);
		for (; j<650; j++)
		{
			if (j == i + 500)
				B(i, j) = 1;
			B(i, j) = 0;
		}
	}//B(500,650)



	Mat imageTest[15];
	imageTest[0] = imread("database\\test\\1.jpg");
	imageTest[1] = imread("database\\test\\2.jpg");
	imageTest[2] = imread("database\\test\\3.jpg");
	imageTest[3] = imread("database\\test\\4.jpg");
	imageTest[4] = imread("database\\test\\5.jpg");
	imageTest[5] = imread("database\\test\\6.jpg");
	imageTest[6] = imread("database\\test\\7.jpg");
	imageTest[7] = imread("database\\test\\8.jpg");
	imageTest[8] = imread("database\\test\\9.jpg");
	imageTest[9] = imread("database\\test\\10.jpg");
	imageTest[10] = imread("database\\test\\11.jpg");
	imageTest[11] = imread("database\\test\\12.jpg");
	imageTest[12] = imread("database\\test\\13.jpg");
	imageTest[13] = imread("database\\test\\14.jpg");
	imageTest[14] = imread("database\\test\\15.jpg");
	int o;

	for (o = 0; o<15; o++) 
	{
		k = 0;
		image = imageTest[o];   //赋值Y矩阵
								//image = imread("database\\001\\02.jpg"); 
		if (image.channels() == 3)
		{
			//若是多通道彩色图，则把图片转换为单通道灰色图
			Mat image1;
			cvtColor(image, image1, CV_BGR2GRAY);
			if (image1.channels() == 1)
				image1.copyTo(image);
		}

		sum = 0;
		for (i = 0; i<image.cols; i++) {
			for (j = 0; j<image.rows; j++) {
				sum = sum + image.at<uchar>(j, i)*image.at<uchar>(j, i);
			}
		}
		sum = sqrt(sum);
		for (i = 0; i<image.cols; i++) {
			for (j = 0; j<image.rows; j++) {
				Y(k++) = image.at<uchar>(j, i) / sum;
			}
		}

		Y1 = R*Y;//Y1(500,1)  R(500,20139)  Y(20139,1)

		sum = 0;
		for (j = 0; j<500; j++)
		{
			sum = sum + Y1(j, 0)*Y1(j, 0);
		}
		sum = sqrt(sum);
		for (j = 0; j<500; j++)
		{
			Y1(j, 0) = Y1(j, 0) / sum;
		}



		kl1p::TMatrixOperator<klab::DoubleReal> * matrix = new kl1p::TMatrixOperator<klab::DoubleReal>(B);
		klab::TSmartPointer<kl1p::TOperator<klab::DoubleReal, klab::DoubleReal> > * B1 = new klab::TSmartPointer<kl1p::TOperator<klab::DoubleReal, klab::DoubleReal> >(matrix);

		klab::DoubleReal tolerance = 1e1;	// Tolerance of the solution.
		kl1p::TBasisPursuitSolver<klab::DoubleReal> bp(tolerance);
		bp.solve(Y1, *B1, W);

		for (i = 0; i<150; i++)
			x1(i) = W(i);
		for (; i<650; i++)
			e1(i - 150) = W(i);

		Y1 = Y1 - e1;


		double r[15];
		arma::Col<klab::DoubleReal> l(150);
		arma::Col<klab::DoubleReal> l1(500);
		for (i = 0; i<15; i++)
		{
			sum = 0;
			for (j = 0; j<150; j++)
				l(j) = 0;
			for (j = i * 10; j<i * 10 + 10; j++)
				l(j) = x1(j);
			l1 = Y1 - A1*l;
			for (j = 0; j<500; j++)
				sum = sum + l1(j)*l1(j);
			sum = sqrt(sum);
			r[i] = sum;
			//cout<<sum<<endl;
		}
		double  min = r[0];
		int min_num = 0;
		for (i = 0; i<15; i++)
		{
			if (r[i]<min)
			{
				min = r[i];
				min_num = i;
			}
		}
		cout << "第" << o + 1 << "张图片与第" << min_num + 1 << "人的脸匹配" << endl;


		double sci, sum1 = 0, sum2 = 0, sum1_max = 0;
		for (i = 0; i<15; i++)
		{
			sum1 = 0;
			for (j = 0; j<10; j++)
				sum1 = sum1 + abs(x1(i * 10 + j));
			if (sum1>sum1_max)
				sum1_max = sum1;
		}
		for (i = 0; i<150; i++)
			sum2 = sum2 + abs(x1(i));
		sci = (sum1_max * 15 / sum2 - 1) / 14;
		cout << "SCI为" << sci << endl;

	}
	getchar();
	return 0;
}

