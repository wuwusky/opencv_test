#include <opencv2/opencv.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void main()
{
	Mat src1 = imread("..//pinjie//test//fail//报错//15-1.jpg", 1);
	Mat src2 = imread("..//pinjie//test//fail//报错//15-2.jpg", 1);
	Mat src3 = imread("..//pinjie//1-3.jpg", 1);
	Mat dst;
	double dScale = 1.0;
	vector<Mat>images;
	resize(src1, src1, Size(), dScale, dScale);
	resize(src2, src2, Size(), dScale, dScale);
	resize(src3, src3, Size(), dScale, dScale);
	images.push_back(src1);
	images.push_back(src2);
	//images.push_back(src3);
	double t = getTickCount();
	cv::Stitcher test = cv::Stitcher::createDefault(false);
	test.setWaveCorrection(false);
	test.setWaveCorrectKind(detail::WAVE_CORRECT_HORIZ);
	test.setBundleAdjuster(makePtr<detail::BundleAdjusterReproj>());
	//test.setBundleAdjuster(makePtr<detail::BundleAdjusterRay>());


	test.setPanoConfidenceThresh(0.10);
	test.setRegistrationResol(0.6);
	test.setSeamEstimationResol(0.1);
	test.setFeaturesMatcher(makePtr<detail::BestOf2NearestMatcher>(false));
	test.setFeaturesFinder(makePtr<detail::OrbFeaturesFinder>());
	test.setWarper(makePtr<SphericalWarper>());
	test.setSeamFinder(makePtr<detail::GraphCutSeamFinder>(detail::GraphCutSeamFinderBase::COST_COLOR));
	test.setExposureCompensator(makePtr<detail::BlocksGainCompensator>());
	test.setBlender(makePtr<detail::MultiBandBlender>(false));




	cv::Stitcher::Status statusError = test.stitch(images, dst);

	t = ((double)getTickCount() - t) / (double)getTickFrequency();
	cout << "耗时：" << t << endl;
	if (dst.empty())
	{
		printf("没成功\n");
		cout << "错误码：" << statusError << endl;
	}
	else
	{
		Mat temp;
		resize(dst, temp, Size(600, 400));
		imshow("2", temp);
	}
	waitKey(0);
	destroyAllWindows();
}