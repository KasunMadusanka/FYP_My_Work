#include <stdio.h>
#include <iostream>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <QtCore>


using namespace cv;
using namespace std;


void getImageDifference();
void getDiffInVideo();


void mainZX()
{


	getDiffInVideo();


}

void getDiffInVideo(){
	//global variables
	Mat frame; //current frame
	Mat resize_blur_Img;
	Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
	Mat binaryImg;
	//Mat TestImg;
	Mat ContourImg; //fg mask fg mask generated by MOG2 method
	Ptr< BackgroundSubtractor> pMOG2; //MOG2 Background subtractor

	pMOG2 = new BackgroundSubtractorMOG2(300, 32, true);//300,0.0);

	char fileName[100] = "C:\\Users\\dehandecroos\\Desktop\\Videos\\PRG6.avi"; //video\\mm2.avi"; //mm2.avi"; //cctv 2.mov"; //mm2.avi"; //";//_p1.avi";
	VideoCapture stream1(fileName);   //0 is the id of video device.0 if you have only one camera

	//morphology element
	Mat element = getStructuringElement(MORPH_RECT, Size(7, 7), Point(3, 3));

	//unconditional loop
	int skippedFrames = 2500;
	for (int i = 0; i < skippedFrames; i++)
	{
		stream1.grab();

	}
	while (true) {
		const clock_t begin_time = clock();
		Mat cameraFrame;
		if (!(stream1.read(frame))) //get one frame form video
			break;
		imshow("Original", frame);
		//cvtColor(frame, frame, CV_BGR2HSV);
		resize_blur_Img = frame;
		//cvtColor(resize_blur_Img, resize_blur_Img, CV_BGR2HSV);
		//Resize
		//resize(frame, resize_blur_Img, Size(frame.size().width/3, frame.size().height/3) );
		//Blur
		blur(resize_blur_Img, resize_blur_Img, Size(4, 4));
		//Background subtraction
		pMOG2->operator()(resize_blur_Img, fgMaskMOG2, -1);//,-0.5);

		///////////////////////////////////////////////////////////////////
		//pre procesing
		//1 point delete
		//morphologyEx(fgMaskMOG2, fgMaskMOG2, CV_MOP_ERODE, element);
		morphologyEx(fgMaskMOG2, binaryImg, CV_MOP_CLOSE, element);
		//morphologyEx(fgMaskMOG2, testImg, CV_MOP_OPEN, element);

		//Shadow delete
		//Binary
		threshold(binaryImg, binaryImg, 128, 255, CV_THRESH_BINARY);

		//Find contour
		ContourImg = binaryImg.clone();
		//less blob delete
		vector< vector< Point> > contours;
		findContours(ContourImg,
			contours, // a vector of contours
			CV_RETR_EXTERNAL, // retrieve the external contours
			CV_CHAIN_APPROX_NONE); // all pixels of each contours

		vector< Rect > output;
		vector< vector< Point> >::iterator itc = contours.begin();
		while (itc != contours.end()) {

			//Create bounding rect of object
			//rect draw on origin image
			Rect mr = boundingRect(Mat(*itc));
			rectangle(resize_blur_Img, mr, CV_RGB(255, 0, 0),2);
			++itc;
		}

		///////////////////////////////////////////////////////////////////
		qDebug() << "time is " << float(clock() - begin_time) / CLOCKS_PER_SEC;
		qDebug() << " ";
		//Display
		imshow("Shadow_Removed", binaryImg);
		imshow("Detected", resize_blur_Img);
		imshow("MOG2", fgMaskMOG2);

		if (waitKey(5) >= 0)
			break;
	}
}


