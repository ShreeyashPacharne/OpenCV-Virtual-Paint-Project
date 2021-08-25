#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;
using namespace cv;


//	GLOBAL VARIABLES

Mat img;
vector<vector<int>> newPoints;

//colors should be detected
//structure of mycolors will be {hmin,smin,vmin,hmax,smax,vmax}
vector<vector<int>> mycolors{ {0,75,149,29,212,255},		//orange
								{55,38,95,89,255,255} };	//green

//these colrs will be drawed whn above gets detected
vector<Scalar> mycolorvalues{ {0,119,255},		//orange
							{0,255,0} };		//green



//GETCONTOURS FUNCTION


Point getcontours(Mat imgdil)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imgdil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//drawContours(img, contours, -1, Scalar(255, 0, 255),2);

	vector<vector<Point>> conpoly(contours.size());
	vector<Rect> boundrect(contours.size());
	
	Point mypoint(0, 0);
	
	for (int i = 0; i < contours.size(); i++)		//this is used to not detect the noise in the image
	{
		int area = contourArea(contours[i]);
		//cout << "Length of Contours" << i << ": " << contours.size() << endl;

		if (area >= 1000)
		{

			float peri = arcLength(contours[i], true);		//for counting the number of curves or points
			approxPolyDP(contours[i], conpoly[i], 0.02 * peri, true);

			//cout << "Length of Conpoly" << i << ": " << conpoly[i].size() << endl;
			boundrect[i] = boundingRect(conpoly[i]);
			//in below will adjust the coordinates as we want to focus on the tip pf the pen
			mypoint.x = boundrect[i].x + boundrect[i].width / 2;
			mypoint.y = boundrect[i].y;


			//drawContours(img, conpoly, i, Scalar(0, 0, 255), 3);
			//rectangle(img, boundrect[i].tl(), boundrect[i].br(), Scalar(0, 255, 0), 5);
			
		}
	}
	return mypoint;
}



//FINDING COLOR FUNCTION


vector<vector<int>> findcolor(Mat img)
{
	Mat imgHSV,mask;
	cvtColor(img, imgHSV, COLOR_BGR2HSV, CV_8UC3);
	for (int i = 0; i < mycolors.size(); i++)
	{
		Scalar lower(mycolors[i][0], mycolors[i][1], mycolors[i][2]);
		Scalar higher(mycolors[i][3], mycolors[i][4], mycolors[i][5]);
		inRange(imgHSV, lower, higher, mask);
		//imshow(to_string(i), mask);
		
		//now we have to send the mask value to the getcontours() function
		Point mypoint = getcontours(mask);
		if (mypoint.x != 0 && mypoint.y != 0)
		{
			newPoints.push_back({ mypoint.x,mypoint.y,i });
		}
	}
	return newPoints;
}



//DRAW FUNCTION


void draw(vector<vector<int>> newPoints , vector<Scalar> mycolorvalues)
{

	for (int i = 0; i < newPoints.size() ; i++)
	{
		circle(img, Point(newPoints[i][0], newPoints[i][1]), 10, mycolorvalues[newPoints[i][2]], FILLED);		
	}

}




// MAIN FUNCTION


void main()
{
	//instead of including path we pass the id number of the webcam generally it is 0
	VideoCapture vp(0);
	

	while (true) {

		vp.read(img);
		newPoints = findcolor(img);
		draw(newPoints, mycolorvalues);

		imshow("Image", img);
		waitKey(10);
	}

}