#include<opencv2/opencv.hpp>
#include<math.h>
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
using namespace cv;
using namespace std;
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void main()
{
	
	cv::Point p1, p2, p3, p4, center;
	cv::Point p11, p22, p33, p44;


	int dx13, dy13;
	dx13 = abs(p1.x - p3.x);
	dy13 = abs(p1.y - p3.y);
	double thre13 = atan((double)((double)dx13 / (double)dy13));


	int RR = abs(p2.y - p4.y);
	double dr1 = sqrt((double)((p1.x - center.x)*(p1.x - center.x) + (p1.y - center.y)*(p1.y - center.y)));
	p11.x = p1.x - (RR - (dr1)*cos(thre13));
	p11.y = p1.y - (RR - (dr1)*sin(thre13));

	double dr3 = sqrt((double)((p3.x - center.x)*(p3.x - center.x) + (p3.y - center.y)*(p3.y - center.y)));
	p33.x = p3.x + (RR - (dr3)*cos(thre13));
	p33.y = p3.y + (RR - (dr3)*sin(thre13));


	int dx24, dy24;
	dx24 = abs(p2.x - p4.x);
	dy24 = abs(p2.y - p4.y);
	double thre24 = atan((double)((double)dx24 / (double)dy24));

	double dr2 = sqrt((double)((p2.x - center.x)*(p2.x - center.x) + (p2.y - center.y)*(p2.y - center.y)));
	p22.x = p2.x - (RR - (dr2)*cos(thre24));
	p22.y = p2.y - (RR - (dr2)*sin(thre24));

	double dr4 = sqrt((double)((p4.x - center.x)*(p4.x - center.x) + (p4.y - center.y)*(p4.y - center.y)));
	p44.x = p4.x + (RR - (dr4)*cos(thre24));
	p44.y = p4.y + (RR - (dr4)*sin(thre24));

}

