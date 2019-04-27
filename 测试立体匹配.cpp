#include "opencv2/surface_matching.hpp"
#include <iostream>
#include "opencv2/surface_matching/ppf_helpers.hpp"
#include "opencv2/core/utility.hpp"
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
using namespace std;
using namespace cv;
using namespace ppf_match_3d;
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void main()
{
#if (defined __x86_64__ || defined _M_X64)
	cout << "Running on 64 bits" << endl;
#else
	cout << "Running on 32 bits" << endl;
#endif

#ifdef _OPENMP
	cout << "Running with OpenMP" << endl;
#else
	cout << "Running without OpenMP and without TBB" << endl;
#endif

	string modelFileName = "parasaurolophus_6700.ply";
	string sceneFileName = "rs22_proc2.ply";

	Mat pc = loadPLYSimple("parasaurolophus_6700.ply", 0);
	//Mat pc = loadPLYSimple(modelFileName.c_str(), 1);

	writePLY(pc, "²âÊÔ1.ply");
	writePLYVisibleNormals(pc, "²âÊÔ2.ply");
	// Now train the model
	cout << "Training..." << endl;
	int64 tick1 = cv::getTickCount();
	ppf_match_3d::PPF3DDetector detector(0.025, 0.05);
	detector.trainModel(pc);
	int64 tick2 = cv::getTickCount();
	cout << endl << "Training complete in "
		<< (double)(tick2-tick1)/ cv::getTickFrequency()
		<< " sec" << endl << "Loading model..." << endl;

	// Read the scene
	Mat pcTest = loadPLYSimple(sceneFileName.c_str(), 1);

	// Match the model to the scene and get the pose
	cout << endl << "Starting matching..." << endl;
	vector<Pose3DPtr> results;
	tick1 = cv::getTickCount();
	detector.match(pcTest, results, 1.0/40.0, 0.05);
	tick2 = cv::getTickCount();
	cout << endl << "PPF Elapsed Time " <<
		(tick2-tick1)/cv::getTickFrequency() << " sec" << endl;

	//check results size from match call above
	size_t results_size = results.size();
	cout << "Number of matching poses: " << results_size;
	if (results_size == 0) {
		cout << endl << "No matching poses found. Exiting." << endl;
		exit(0);
	}

	// Get only first N results - but adjust to results size if num of results are less than that specified by N
	size_t N = 2;
	if (results_size < N) {
		cout << endl << "Reducing matching poses to be reported (as specified in code): "
			<< N << " to the number of matches found: " << results_size << endl;
		N = results_size;
	}
	vector<Pose3DPtr> resultsSub(results.begin(),results.begin()+N);

	// Create an instance of ICP
	ICP icp(100, 0.005f, 2.5f, 8);
	int64 t1 = cv::getTickCount();

	// Register for all selected poses
	cout << endl << "Performing ICP on " << N << " poses..." << endl;
	icp.registerModelToScene(pc, pcTest, resultsSub);
	int64 t2 = cv::getTickCount();

	cout << endl << "ICP Elapsed Time " <<
		(t2-t1)/cv::getTickFrequency() << " sec" << endl;

	cout << "Poses: " << endl;
	// debug first five poses
	for (size_t i=0; i<resultsSub.size(); i++)
	{
		Pose3DPtr result = resultsSub[i];
		cout << "Pose Result " << i << endl;
		result->printPose();
		if (i==0)
		{
			Mat pct = transformPCPose(pc, result->pose);
			writePLYVisibleNormals(pct, "para6700PCTrans.ply");
		}
		else
		{
			Mat pct = transformPCPose(pc, result->pose);
			writePLYVisibleNormals(pct, "para6700PCTrans222.ply");

		}
	}


}
