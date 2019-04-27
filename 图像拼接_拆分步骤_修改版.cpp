#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "opencv2/stitching/detail/autocalib.hpp"
#include "opencv2/stitching/detail/blenders.hpp"
#include "opencv2/stitching/detail/timelapsers.hpp"
#include "opencv2/stitching/detail/camera.hpp"
#include "opencv2/stitching/detail/exposure_compensate.hpp"
#include "opencv2/stitching/detail/matchers.hpp"
#include "opencv2/stitching/detail/motion_estimators.hpp"
#include "opencv2/stitching/detail/seam_finders.hpp"
#include "opencv2/stitching/detail/util.hpp"
#include "opencv2/stitching/detail/warpers.hpp"
#include "opencv2/stitching/warpers.hpp"
#include <opencv2/xfeatures2d.hpp>
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
using namespace cv::detail;
//------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------
// Default command line args
bool preview = false;
bool try_cuda = false;
double work_megapix = 0.6;
double seam_megapix = 0.1;
double compose_megapix = -1;
float conf_thresh = 0.3f;
float match_conf = 0.6f;
string features_type = "surf";//特征点类型
string ba_cost_func = "ray";//
string ba_refine_mask = "xxxxx";
bool do_wave_correct = true;
WaveCorrectKind wave_correct = detail::WAVE_CORRECT_HORIZ;
bool save_graph = false;
std::string save_graph_to;
string warp_type = "plane";//相机类型
int expos_comp_type = ExposureCompensator::GAIN_BLOCKS;
string seam_find_type = "gc_color";
int blend_type = Blender::MULTI_BAND;
int timelapse_type = Timelapser::AS_IS;
float blend_strength = 5;
string result_name = "result.jpg";
bool timelapse = false;
int range_width = -1;

int main(int argc, char* argv[])
{
	Mat src1 = imread("..//pinjie//13-1.jpg", 1);
	Mat src2 = imread("..//pinjie//13-2.jpg", 1);
	Mat dst;
	Mat srcG1, srcG2;
	double dScale = 0.5;
	vector<Mat>imagesS;
	vector<Mat>imagesGray;

	resize(src1, src1, Size(), dScale, dScale);
	resize(src2, src2, Size(), dScale, dScale);
	imagesS.push_back(src1);
	imagesS.push_back(src2);
	cvtColor(src1, srcG1, COLOR_BGR2GRAY);
	cvtColor(src2, srcG2, COLOR_BGR2GRAY);
	imagesGray.push_back(srcG1);
	imagesGray.push_back(srcG2);
	int num_images = imagesS.size();


	double work_scale = 1.0, seam_scale = 1.0, compose_scale = 1.0;
	bool is_work_scale_set = false, is_seam_scale_set = false, is_compose_scale_set = false;

	Mat full_img;
	vector<ImageFeatures> features(num_images);
	vector<Mat> images(num_images);
	vector<Size> full_img_sizes(num_images);
	double seam_work_aspect = 1;
	//Ptr<cv::FastFeatureDetector> detector = FastFeatureDetector::create(5, false, FastFeatureDetector::TYPE_7_12);
	//Ptr<cv::xfeatures2d::FREAK> descriptor = cv::xfeatures2d::FREAK::create(false, false);
	Ptr<cv::xfeatures2d::SurfFeatureDetector> detector = cv::xfeatures2d::SurfFeatureDetector::create(100,4, 3, true, true);
	Ptr<cv::xfeatures2d::SurfDescriptorExtractor> descriptor = cv::xfeatures2d::SurfDescriptorExtractor::create(400, 4, 3, true, true);


	for (int i = 0; i < num_images; ++i)
	{
		full_img = imagesGray[i];
		full_img_sizes[i] = full_img.size();
		features[i].img_idx = i;
		features[i].img_size = full_img.size();
		images[i] = full_img.clone();
		detector->detect(images[i], features[i].keypoints);
		descriptor->compute(images[i], features[i].keypoints, features[i].descriptors);

	}
	full_img.release();

	//-----测试下改用之前的match方式
	vector<DMatch> pairwise_matchesMy_0_1, pairwise_matchesMy_1_0;
	BFMatcher matcherMy(NORM_L2, true);
	matcherMy.match(features[0].descriptors, features[1].descriptors, pairwise_matchesMy_0_1);
	matcherMy.match(features[1].descriptors, features[0].descriptors, pairwise_matchesMy_1_0);


	double min_dist_0_1 = INT_MAX;
	double min_dist_1_0 = INT_MAX;
	vector<DMatch> good_matches_0_1, good_matches_1_0;

	for (int i = 0; i < pairwise_matchesMy_0_1.size(); i++)
	{
		double dist = pairwise_matchesMy_0_1[i].distance;
		if (dist < min_dist_0_1)
		{
			min_dist_0_1 = dist;
		}
	}
	for (int i = 0; i < pairwise_matchesMy_1_0.size(); i++)
	{
		double dist = pairwise_matchesMy_1_0[i].distance;
		if (dist < min_dist_1_0)
		{
			min_dist_1_0 = dist;
		}
	}

	for (int i = 0; i < pairwise_matchesMy_0_1.size(); i++)
	{
		if (pairwise_matchesMy_0_1[i].distance <= 5 * min_dist_0_1 && pairwise_matchesMy_0_1[i].distance <= 500)
		{
			good_matches_0_1.push_back(pairwise_matchesMy_0_1[i]);
		}
	}
	for (int i = 0; i < pairwise_matchesMy_1_0.size(); i++)
	{
		if (pairwise_matchesMy_1_0[i].distance <= 5 * min_dist_1_0 && pairwise_matchesMy_1_0[i].distance <= 500)
		{
			good_matches_1_0.push_back(pairwise_matchesMy_1_0[i]);
		}
	}


	vector<uchar> inlinesMask_0_1;
	vector<uchar> inlinesMask_1_0;
	int numInline_0_1 = 0;
	int numInline_1_0 = 0;

	vector<Point2f> vecP0, vecP1;
	cv::Mat H_0_1, H_1_0;

	for (int i = 0; i < good_matches_0_1.size();i++)
	{
		vecP0.push_back(features[0].keypoints[good_matches_0_1[i].queryIdx].pt);
		vecP1.push_back(features[1].keypoints[good_matches_0_1[i].trainIdx].pt);
	}
	H_0_1 = findHomography(vecP0, vecP1, RANSAC, 3.0, inlinesMask_0_1);

	vecP0.clear();
	vecP1.clear();

	for (int i = 0; i < good_matches_1_0.size(); i++)
	{
		vecP0.push_back(features[0].keypoints[good_matches_1_0[i].trainIdx].pt);
		vecP1.push_back(features[1].keypoints[good_matches_1_0[i].queryIdx].pt);
	}
	H_1_0 = findHomography(vecP1, vecP0, RANSAC, 3.0, inlinesMask_1_0);


	vector<DMatch> good_matches_end_0_1, good_matches_end_1_0;
	for (int i = 0; i < good_matches_0_1.size(); i++)
	{
		if (inlinesMask_0_1[i] == 1)
		{
			good_matches_end_0_1.push_back(good_matches_0_1[i]);
			numInline_0_1++;
		}
	}
	for (int i = 0; i < good_matches_1_0.size(); i++)
	{
		if (inlinesMask_1_0[i] == 1)
		{
			good_matches_end_1_0.push_back(good_matches_1_0[i]);
			numInline_1_0++;
		}
	}



	cv::Mat temp;
	drawMatches(imagesS[0], features[0].keypoints, imagesS[1], features[1].keypoints, good_matches_end_0_1, temp, Scalar(0, 255, 255));
	imshow("特征点的配准结果1", temp);
	waitKey(0);
	//drawMatches(imagesS[1], features[1].keypoints, imagesS[0], features[0].keypoints, good_matches_end_1_0, temp, Scalar(0, 255, 255));
	//imshow("特征点的配准结果2", temp);
	//waitKey(0);

	vector<MatchesInfo> pairwise_matches(4);

	pairwise_matches[1].confidence = 1.0;
	pairwise_matches[1].src_img_idx = 0;
	pairwise_matches[1].dst_img_idx = 1;
	pairwise_matches[1].H = H_0_1;
	pairwise_matches[1].inliers_mask = inlinesMask_0_1;
	pairwise_matches[1].matches = good_matches_0_1;
	pairwise_matches[1].num_inliers = numInline_0_1;

	pairwise_matches[2].confidence = 1.0;
	pairwise_matches[2].src_img_idx = 0;
	pairwise_matches[2].dst_img_idx = 1;
	pairwise_matches[2].H = H_0_1;
	pairwise_matches[2].inliers_mask = inlinesMask_0_1;
	pairwise_matches[2].matches = good_matches_0_1;
	pairwise_matches[2].num_inliers = numInline_0_1;
	//pairwise_matches[2].confidence = 1.0;
	//pairwise_matches[2].src_img_idx = 1;
	//pairwise_matches[2].dst_img_idx = 0;
	//pairwise_matches[2].H = H_1_0;
	//pairwise_matches[2].inliers_mask = inlinesMask_1_0;
	//pairwise_matches[2].matches = good_matches_1_0;
	//pairwise_matches[1].num_inliers = numInline_1_0;



	pairwise_matches[0].confidence = 0.0;
	pairwise_matches[0].src_img_idx = -1;
	pairwise_matches[0].dst_img_idx = -1;
	pairwise_matches[3].confidence = 0.0;
	pairwise_matches[3].src_img_idx = -1;
	pairwise_matches[3].dst_img_idx = -1;



	//估计相机的内参
	HomographyBasedEstimator estimator;
	vector<CameraParams> cameras;
	if (!estimator(features, pairwise_matches, cameras))
	{
		cout << "Homography estimation failed.\n";
		return -1;
	}

	for (size_t i = 0; i < cameras.size(); ++i)
	{
		Mat R;
		cameras[i].R.convertTo(R, CV_32F);
		cameras[i].R = R;
	}

	Ptr<detail::BundleAdjusterBase> adjuster;
	if (ba_cost_func == "reproj") adjuster = makePtr<detail::BundleAdjusterReproj>();
	else if (ba_cost_func == "ray") adjuster = makePtr<detail::BundleAdjusterRay>();
	else
	{
		cout << "Unknown bundle adjustment cost function: '" << ba_cost_func << "'.\n";
		return -1;
	}
	adjuster->setConfThresh(conf_thresh);
	Mat_<uchar> refine_mask = Mat::zeros(3, 3, CV_8U);
	if (ba_refine_mask[0] == 'x') refine_mask(0, 0) = 1;
	if (ba_refine_mask[1] == 'x') refine_mask(0, 1) = 1;
	if (ba_refine_mask[2] == 'x') refine_mask(0, 2) = 1;
	if (ba_refine_mask[3] == 'x') refine_mask(1, 1) = 1;
	if (ba_refine_mask[4] == 'x') refine_mask(1, 2) = 1;
	adjuster->setRefinementMask(refine_mask);
	//if (!(*adjuster)(features, pairwise_matches, cameras))
	//{
	//	cout << "Camera parameters adjusting failed.\n";
	//	return -1;
	//}

	// Find median focal length

	vector<double> focals;
	for (size_t i = 0; i < cameras.size(); ++i)
	{
		focals.push_back(cameras[i].focal);
	}

	sort(focals.begin(), focals.end());
	float warped_image_scale;
	if (focals.size() % 2 == 1)
		warped_image_scale = static_cast<float>(focals[focals.size() / 2]);
	else
		warped_image_scale = static_cast<float>(focals[focals.size() / 2 - 1] + focals[focals.size() / 2]) * 0.5f;

	if (do_wave_correct)
	{
		vector<Mat> rmats;
		for (size_t i = 0; i < cameras.size(); ++i)
			rmats.push_back(cameras[i].R.clone());
		waveCorrect(rmats, wave_correct);
		for (size_t i = 0; i < cameras.size(); ++i)
			cameras[i].R = rmats[i];
	}

	vector<Point> corners(num_images);
	vector<UMat> masks_warped(num_images);
	vector<UMat> images_warped(num_images);
	vector<Size> sizes(num_images);
	vector<UMat> masks(num_images);

	// Preapre images masks
	for (int i = 0; i < num_images; ++i)
	{
		masks[i].create(imagesS[i].size(), CV_8U);
		masks[i].setTo(Scalar::all(255));
	}

	// Warp images and their masks

	Ptr<WarperCreator> warper_creator;

	if (warp_type == "plane")
		warper_creator = makePtr<cv::PlaneWarper>();
	else if (warp_type == "cylindrical")
		warper_creator = makePtr<cv::CylindricalWarper>();
	else if (warp_type == "spherical")
		warper_creator = makePtr<cv::SphericalWarper>();
	else if (warp_type == "fisheye")
		warper_creator = makePtr<cv::FisheyeWarper>();
	else if (warp_type == "stereographic")
		warper_creator = makePtr<cv::StereographicWarper>();
	else if (warp_type == "compressedPlaneA2B1")
		warper_creator = makePtr<cv::CompressedRectilinearWarper>(2.0f, 1.0f);
	else if (warp_type == "compressedPlaneA1.5B1")
		warper_creator = makePtr<cv::CompressedRectilinearWarper>(1.5f, 1.0f);
	else if (warp_type == "compressedPlanePortraitA2B1")
		warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(2.0f, 1.0f);
	else if (warp_type == "compressedPlanePortraitA1.5B1")
		warper_creator = makePtr<cv::CompressedRectilinearPortraitWarper>(1.5f, 1.0f);
	else if (warp_type == "paniniA2B1")
		warper_creator = makePtr<cv::PaniniWarper>(2.0f, 1.0f);
	else if (warp_type == "paniniA1.5B1")
		warper_creator = makePtr<cv::PaniniWarper>(1.5f, 1.0f);
	else if (warp_type == "paniniPortraitA2B1")
		warper_creator = makePtr<cv::PaniniPortraitWarper>(2.0f, 1.0f);
	else if (warp_type == "paniniPortraitA1.5B1")
		warper_creator = makePtr<cv::PaniniPortraitWarper>(1.5f, 1.0f);
	else if (warp_type == "mercator")
		warper_creator = makePtr<cv::MercatorWarper>();
	else if (warp_type == "transverseMercator")
		warper_creator = makePtr<cv::TransverseMercatorWarper>();


	if (!warper_creator)
	{
		cout << "Can't create the following warper '" << warp_type << "'\n";
		return 1;
	}

	Ptr<RotationWarper> warper = warper_creator->create(static_cast<float>(warped_image_scale * seam_work_aspect));

	for (int i = 0; i < num_images; ++i)
	{
		Mat_<float> K;
		cameras[i].K().convertTo(K, CV_32F);
		float swa = (float)seam_work_aspect;
		K(0, 0) *= swa; K(0, 2) *= swa;
		K(1, 1) *= swa; K(1, 2) *= swa;

		corners[i] = warper->warp(imagesS[i], K, cameras[i].R, INTER_LINEAR, BORDER_REFLECT, images_warped[i]);



		sizes[i] = images_warped[i].size();

		warper->warp(masks[i], K, cameras[i].R, INTER_NEAREST, BORDER_CONSTANT, masks_warped[i]);
	}

	cout << images_warped[0].channels() << endl;

	vector<UMat> images_warped_f(num_images);
	for (int i = 0; i < num_images; ++i)
	{
		images_warped[i].convertTo(images_warped_f[i], CV_32FC3);
	}

	cout << images_warped_f[0].channels() << endl;



	Ptr<ExposureCompensator> compensator = ExposureCompensator::createDefault(expos_comp_type);
	compensator->feed(corners, images_warped, masks_warped);

	Ptr<SeamFinder> seam_finder;
	if (seam_find_type == "no")
		seam_finder = makePtr<detail::NoSeamFinder>();
	else if (seam_find_type == "voronoi")
		seam_finder = makePtr<detail::VoronoiSeamFinder>();
	else if (seam_find_type == "gc_color")
	{
		seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR);
	}
	else if (seam_find_type == "gc_colorgrad")
	{
		seam_finder = makePtr<detail::GraphCutSeamFinder>(GraphCutSeamFinderBase::COST_COLOR_GRAD);
	}
	else if (seam_find_type == "dp_color")
		seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR);
	else if (seam_find_type == "dp_colorgrad")
		seam_finder = makePtr<detail::DpSeamFinder>(DpSeamFinder::COLOR_GRAD);
	if (!seam_finder)
	{
		cout << "Can't create the following seam finder '" << seam_find_type << "'\n";
		return 1;
	}

	cout << images_warped_f[0].channels() << endl;

	seam_finder->find(images_warped_f, corners, masks_warped);
	// Release unused memory
	images.clear();
	images_warped.clear();
	images_warped_f.clear();
	masks.clear();


	Mat img_warped, img_warped_s;
	Mat dilated_mask, seam_mask, mask, mask_warped;
	Mat img;
	Ptr<Blender> blender;
	Ptr<Timelapser> timelapser;
	//double compose_seam_aspect = 1;
	double compose_work_aspect = 1;

	for (int img_idx = 0; img_idx < num_images; ++img_idx)
	{
		// Read image and resize it if necessary
		full_img = imagesS[img_idx];
		if (!is_compose_scale_set)
		{
			if (compose_megapix > 0)
				compose_scale = min(1.0, sqrt(compose_megapix * 1e6 / full_img.size().area()));
			is_compose_scale_set = true;

			// Compute relative scales
			//compose_seam_aspect = compose_scale / seam_scale;
			compose_work_aspect = compose_scale / work_scale;

			// Update warped image scale
			warped_image_scale *= static_cast<float>(compose_work_aspect);
			warper = warper_creator->create(warped_image_scale);

			// Update corners and sizes
			for (int i = 0; i < num_images; ++i)
			{
				// Update intrinsics
				cameras[i].focal *= compose_work_aspect;
				cameras[i].ppx *= compose_work_aspect;
				cameras[i].ppy *= compose_work_aspect;

				// Update corner and size
				Size sz = full_img_sizes[i];
				if (std::abs(compose_scale - 1) > 1e-1)
				{
					sz.width = cvRound(full_img_sizes[i].width * compose_scale);
					sz.height = cvRound(full_img_sizes[i].height * compose_scale);
				}

				Mat K;
				cameras[i].K().convertTo(K, CV_32F);
				Rect roi = warper->warpRoi(sz, K, cameras[i].R);
				corners[i] = roi.tl();
				sizes[i] = roi.size();
			}
		}

		img = full_img;
		Size img_size = full_img.size();

		Mat K;
		cameras[img_idx].K().convertTo(K, CV_32F);

		// Warp the current image
		warper->warp(img, K, cameras[img_idx].R, INTER_LINEAR, BORDER_REFLECT, img_warped);


		// Warp the current image mask
		mask.create(img_size, CV_8U);
		mask.setTo(Scalar::all(255));
		warper->warp(mask, K, cameras[img_idx].R, INTER_NEAREST, BORDER_CONSTANT, mask_warped);

		// Compensate exposure
		compensator->apply(img_idx, corners[img_idx], img_warped, mask_warped);

		img_warped.convertTo(img_warped_s, img_warped.type());
		img_warped.release();
		full_img.release();
		mask.release();

		//imshow("临时", img_warped_s);
		//imshow("掩膜", mask_warped);


		dilate(masks_warped[img_idx], dilated_mask, Mat());
		resize(dilated_mask, seam_mask, mask_warped.size());
		mask_warped = seam_mask & mask_warped;

		if (!blender && !timelapse)
		{
			blender = Blender::createDefault(blend_type, try_cuda);
			Size dst_sz = resultRoi(corners, sizes).size();
			float blend_width = sqrt(static_cast<float>(dst_sz.area())) * blend_strength / 100.f;
			if (blend_width < 1.f)
				blender = Blender::createDefault(Blender::NO, try_cuda);
			else if (blend_type == Blender::MULTI_BAND)
			{
				MultiBandBlender* mb = dynamic_cast<MultiBandBlender*>(blender.get());
				mb->setNumBands(static_cast<int>(ceil(log(blend_width) / log(2.)) - 1.));
			}
			else if (blend_type == Blender::FEATHER)
			{
				FeatherBlender* fb = dynamic_cast<FeatherBlender*>(blender.get());
				fb->setSharpness(1.f / blend_width);
			}
			blender->prepare(corners, sizes);
		}
		else if (!timelapser && timelapse)
		{
			timelapser = Timelapser::createDefault(timelapse_type);
			timelapser->initialize(corners, sizes);
		}

		if (timelapse)
		{
			//timelapser->process(img_warped_s, Mat::ones(img_warped_s.size(), CV_8UC1), corners[img_idx]);
			//String fixedFileName;
			//size_t pos_s = String(img_names[img_idx]).find_last_of("/\\");
			//if (pos_s == String::npos)
			//{
			//	fixedFileName = "fixed_" + img_names[img_idx];
			//}
			//else
			//{
			//	fixedFileName = "fixed_" + String(img_names[img_idx]).substr(pos_s + 1, String(img_names[img_idx]).length() - pos_s);
			//}
			//imwrite(fixedFileName, timelapser->getDst());
		}
		else
		{
			blender->feed(img_warped_s, mask_warped, corners[img_idx]);
		}

	}

	if (!timelapse)
	{
		Mat result, result_mask;
		blender->blend(result, result_mask);

		imwrite(result_name, result);
		Mat temp;
		result.convertTo(temp, CV_8UC3);
		imshow("结果", temp);
		//imshow("结果", result);


		//circle(result_mask, corners[0], 3, Scalar(0, 0, 0), 2);
		//circle(result_mask, corners[1], 3, Scalar(0, 0, 0), 2);
		//imshow("结果掩膜", result_mask);

		waitKey(0);
	}
	return 0;
}
