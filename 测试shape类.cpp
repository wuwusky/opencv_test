/*
 * shape_context.cpp -- Shape context demo for shape matching
 */

#include "opencv2/shape.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/utility.hpp>
#include <iostream>
#include <string>
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
using namespace std;
using namespace cv;
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
static vector<Point> simpleContour( const Mat& currentQuery, int n=300 , bool flag = true)
{
    vector<vector<Point> > _contoursQuery;
    vector <Point> contoursQuery;

	int pointNum = 0;
    findContours(currentQuery, _contoursQuery, RETR_LIST, CHAIN_APPROX_NONE);
    for (size_t border=0; border<_contoursQuery.size(); border++)
    {
        for (size_t p=0; p<_contoursQuery[border].size(); p++)
        {
            contoursQuery.push_back( _contoursQuery[border][p] );
			pointNum ++;
        }
    }

    // In case actual number of points is less than n
    int dummy=0;
    for (int add=(int)contoursQuery.size()-1; add<n; add++)
    {
        contoursQuery.push_back(contoursQuery[dummy++]); //adding dummy values
    }

     //Ë³Ðò´òÂÒ£¬´ïµ½¾ùÔÈÈ¡ÑùµÄÐ§¹û
	if (flag)
	{
		random_shuffle(contoursQuery.begin(), contoursQuery.end());
	}

    vector<Point> cont;
	if (flag)
	{
		for (int i=0; i<n; i++)
		{
			cont.push_back(contoursQuery[i]);
		}

	}
	else
	{
		for (int i=0; i<pointNum; i++)
		{
			cont.push_back(contoursQuery[i]);
		}

	}
    return cont;
}

int main()
{
	bool shapeContextFlag = false;

	if (shapeContextFlag)
	{
		string path = "../data/shape_sample/";
		cv::Ptr <cv::ShapeContextDistanceExtractor> mysc = cv::createShapeContextDistanceExtractor();
		//cv::Ptr <cv::HausdorffDistanceExtractor> mysc = cv::createHausdorffDistanceExtractor();

		Size sz2Sh(300,300);
		stringstream queryName;
		queryName<<path<<16<<".png";
		Mat query=imread(queryName.str(), IMREAD_GRAYSCALE);
		Mat queryToShow;
		resize(query, queryToShow, sz2Sh);
		imshow("Ä£°å", queryToShow);
		moveWindow("²âÊÔ", 0,0);
		vector<Point> contQuery = simpleContour(query, 100, shapeContextFlag);
		int bestMatch = 0;
		float bestDis=FLT_MAX;
		for ( int ii=1; ii<=20; ii++ )
		{
			if (ii==16) continue;
			waitKey(30);
			stringstream iiname;
			iiname<<path<<ii<<".png";
			cout<<"name: "<<iiname.str()<<endl;
			Mat iiIm=imread(iiname.str(), 0);
			Mat iiToShow;
			resize(iiIm, iiToShow, sz2Sh);
			imshow("²âÊÔ", iiToShow);
			moveWindow("²âÊÔ", sz2Sh.width+50,0);
			vector<Point> contii = simpleContour(iiIm, 300, shapeContextFlag);
			float dis = mysc->computeDistance( contQuery, contii );
			if ( dis<bestDis )
			{
				bestMatch = ii;
				bestDis = dis;
			}
			std::cout<<" distance between "<<queryName.str()<<" and "<<iiname.str()<<" is: "<<dis<<std::endl;
		}
		destroyWindow("²âÊÔ");
		stringstream bestname;
		bestname<<path<<bestMatch<<".png";
		Mat iiIm=imread(bestname.str(), 0);
		Mat bestToShow;
		resize(iiIm, bestToShow, sz2Sh);
		imshow("×î¼ÑÆ¥Åä", bestToShow);
		waitKey(0);
		return 0;

	}
	else
	{
		string path = "../data/shape_sample/";

		Size sz2Sh(300,300);
		stringstream queryName;
		queryName<<path<<16<<".png";
		Mat query=imread(queryName.str(), IMREAD_GRAYSCALE);
		Mat queryToShow;
		resize(query, queryToShow, sz2Sh);
		imshow("Ä£°å", queryToShow);
		moveWindow("TEST", 0,0);
		vector<Point> contQuery = simpleContour(query, 300, shapeContextFlag);
		int bestMatch = 0;
		float bestDis=FLT_MAX;
		for ( int ii=1; ii<=20; ii++ )
		{
			if (ii==16) continue;
			waitKey(30);
			stringstream iiname;
			iiname<<path<<ii<<".png";
			cout<<"name: "<<iiname.str()<<endl;
			Mat iiIm=imread(iiname.str(), 0);
			Mat iiToShow;
			resize(iiIm, iiToShow, sz2Sh);
			imshow("TEST", iiToShow);
			moveWindow("TEST", sz2Sh.width+150,0);
			vector<Point> contii = simpleContour(iiIm, 300, shapeContextFlag);
			float dis = matchShapes( contQuery, contii ,CV_CONTOURS_MATCH_I3, 1);
			if ( dis<bestDis )
			{
				bestMatch = ii;
				bestDis = dis;
			}
			std::cout<<" distance between "<<queryName.str()<<" and "<<iiname.str()<<" is: "<<dis<<std::endl;
		}
		destroyWindow("TEST");
		stringstream bestname;
		bestname<<path<<bestMatch<<".png";
		Mat iiIm=imread(bestname.str(), 0);
		Mat bestToShow;
		resize(iiIm, bestToShow, sz2Sh);
		imshow("×î¼ÑÆ¥Åä", bestToShow);
		waitKey(0);
		return 0;

	}
}
