#include "opencv2/bgsegm.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/core/utility.hpp>
#include <string>
#include <iostream>
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
using namespace cv;
using namespace cv::bgsegm;
using namespace std;
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
static void help()
{

    cout <<"Using OpenCV version " << CV_VERSION << "\n"<<std::endl;
}

int main(int argc, char** argv)
{
    help();

    setUseOptimized(true);
    setNumThreads(8);

    Ptr<BackgroundSubtractor> fgbg = createBackgroundSubtractorGMG(10, 0.8);
    if (!fgbg)
    {
        std::cerr << "Failed to create BackgroundSubtractor.GMG Algorithm." << std::endl;
        return -1;
    }


    VideoCapture cap("2.avi");
    //if (argc > 1)
    //    cap.open(argv[1]);
    //else
    //    cap.open(0);

    if (!cap.isOpened())
    {
        std::cerr << "Cannot read video. Try moving video file to sample directory." << std::endl;
        return -1;
    }

    Mat frame, fgmask, segm;

    namedWindow("FG Segmentation", WINDOW_NORMAL);

    for (;;)
    {
        cap >> frame;

        if (frame.empty())
            break;

        fgbg->apply(frame, fgmask);

        frame.convertTo(segm, CV_8U, 0.5);
        add(frame, Scalar(0, 0, 255), segm, fgmask);

        imshow("FG Segmentation", segm);

        int c = waitKey(30);
        //if (c == 'q' || c == 'Q' || (c & 255) == 27)
        //    break;
    }

    return 0;
}
