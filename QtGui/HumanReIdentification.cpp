//
//  ProBlobDetection.cpp
//  OpencvTut
//
//  Created by Dehan on 4/20/15.
//  Copyright (c) 2015 nephelium. All rights reserved.
//
#include <iostream>

#include "mysql_connection.h"
#include "mysql_driver.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <QtWidgets/QApplication>

#include <QtCore\QFileInfo>
#include <QtWidgets\qlabel.h>
#include <fstream>
#include <sys/stat.h>
#include <QtCore\QCoreApplication>
#include <QtCore\qdir.h>
#include <QtCore\QDebug>
#include <QtCore\QString>
#include <QtCore\qdatetime.h>
#include <stdlib.h>


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/video/background_segm.hpp>

#include "BlobDetection.h"
#include "humanhits.h"


#include <Windows.h>
#include <process.h>

#include <stdio.h>
#include <time.h>
using namespace cv;
using namespace std;
void getDiffInVideo();
void drawHist();
Mat drawHist(Mat greyImage);
Mat src;
Mat dst;
Mat resizedFrame;
Mat processedFrame;
Mat edgeDetected;

Mat savedImage;
Mat savedHist;
RNG rng(12345);


///Removes any dir if exists
void make_directory(QString path);

void make_directory(QString path){

	QDir directory(path);
	if (!directory.exists())
	{
		qDebug() << "Creating " << directory << "directory";
		directory.mkpath(path);
	}
	else
	{
		qDebug() << path << " already exists";
		directory.setNameFilters(QStringList() << "*.*");
		directory.setFilter(QDir::Files);
		foreach(QString dirFile, directory.entryList())
		{
			directory.remove(dirFile);
		}
	}
}

//global variables
Mat frame; //current frame

int frameCounter = 0;

void segmentImage(Mat src)
{
	// Change the background from white to black, since that will help later to extract
	// better results during the use of Distance Transform
	//for (int x = 0; x < src.rows; x++) {
	//	for (int y = 0; y < src.cols; y++) {
	//		if (src.at<Vec3b>(x, y) == Vec3b(255, 255, 255)) {
	//			src.at<Vec3b>(x, y)[0] = 0;
	//			src.at<Vec3b>(x, y)[1] = 0;
	//			src.at<Vec3b>(x, y)[2] = 0;
	//		}
	//	}
	//}
	// Show output image
	//imshow("Black Background Image", src);
	// Create a kernel that we will use for accuting/sharpening our image
	Mat kernel = (Mat_<float>(3, 3) <<
		1, 1, 1,
		1, -8, 1,
		1, 1, 1); // an approximation of second derivative, a quite strong kernel
	// do the laplacian filtering as it is
	// well, we need to convert everything in something more deeper then CV_8U
	// because the kernel has some negative values,
	// and we can expect in general to have a Laplacian image with negative values
	// BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
	// so the possible negative number will be truncated
	Mat imgLaplacian;
	Mat sharp = src; // copy source image to another temporary one
	filter2D(sharp, imgLaplacian, CV_32F, kernel);
	src.convertTo(sharp, CV_32F);
	Mat imgResult = sharp - imgLaplacian;
	// convert back to 8bits gray scale
	imgResult.convertTo(imgResult, CV_8UC3);
	imgLaplacian.convertTo(imgLaplacian, CV_8UC3);
	// imshow( "Laplace Filtered Image", imgLaplacian );
	//imshow("New Sharped Image", imgResult);
	src = imgResult; // copy back
	// Create binary image from source image
	Mat bw;
	cvtColor(src, bw, CV_BGR2GRAY);
	threshold(bw, bw, 40, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//imshow("Binary Image", bw);
	// Perform the distance transform algorithm
	Mat dist;
	distanceTransform(bw, dist, CV_DIST_L2, 3);
	// Normalize the distance image for range = {0.0, 1.0}
	// so we can visualize and threshold it
	normalize(dist, dist, 0, 1., NORM_MINMAX);
	//imshow("Distance Transform Image", dist);
	// Threshold to obtain the peaks
	// This will be the markers for the foreground objects
	threshold(dist, dist, .4, 1., CV_THRESH_BINARY);
	// Dilate a bit the dist image
	Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
	dilate(dist, dist, kernel1);
	//imshow("Peaks", dist);
	// Create the CV_8U version of the distance image
	// It is needed for findContours()
	Mat dist_8u;
	dist.convertTo(dist_8u, CV_8U);
	// Find total markers
	vector<vector<Point> > contours;
	findContours(dist_8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	// Create the marker image for the watershed algorithm
	Mat markers = Mat::zeros(dist.size(), CV_32SC1);
	// Draw the foreground markers
	for (size_t i = 0; i < contours.size(); i++)
		drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i)+1), -1);
	// Draw the background marker
	circle(markers, Point(5, 5), 3, CV_RGB(255, 255, 255), -1);
	//imshow("Markers", markers * 10000);
	// Perform the watershed algorithm
	watershed(src, markers);
	Mat mark = Mat::zeros(markers.size(), CV_8UC1);
	markers.convertTo(mark, CV_8UC1);
	bitwise_not(mark, mark);
	//    imshow("Markers_v2", mark); // uncomment this if you want to see how the mark
	// image looks like at that point
	// Generate random colors
	vector<Vec3b> colors;
	for (size_t i = 0; i < contours.size(); i++)
	{
		int b = theRNG().uniform(0, 255);
		int g = theRNG().uniform(0, 255);
		int r = theRNG().uniform(0, 255);
		colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
	}
	// Create the result image
	Mat dst = Mat::zeros(markers.size(), CV_8UC3);
	// Fill labeled objects with random colors
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i, j);
			if (index > 0 && index <= static_cast<int>(contours.size()))
				dst.at<Vec3b>(i, j) = colors[index - 1];
			else
				dst.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
		}
	}

}

int mainRI()
{

	

	BlobDetection blb;
	QString path = "C:\\Projects\\Output";
	QString path_correct = "C:\\Projects\\Output\\file1\\correct";
	QString path_non = "C:\\Projects\\Output\\file1\\non";
	make_directory(path_correct);
	make_directory(path_non);
	Mat frame; //current frame
	Mat fgMaskMOG2; //fg mask fg mask generated by MOG2 method
	Ptr< BackgroundSubtractor> pMOG2Pointer; //MOG2 Background subtractor
	pMOG2Pointer = new BackgroundSubtractorMOG2(300, 32, true);//300,0.0);
	string prefix = "C:\\AdaptiveCameraNetworkPack\\Videos\\";
	string files[] = {	"PRG6.avi",
						"PRG1.avi", 
						"PRG28.avi",
						"PRG22.avi",
						"PRG7.avi",
						"PRG14.avi" ,
						"PRG23.avi" ,
						"PRG29.avi" };
	for (string file : files){

		string fileName = prefix + file;

		VideoCapture stream1(fileName);

		//morphology element
		Mat element = getStructuringElement(MORPH_RECT, Size(7, 7), Point(3, 3));
		bool flag = true;
		cvNamedWindow("Detected Blob", CV_WINDOW_NORMAL);
		cvSetWindowProperty("Detected Blob", CV_WND_PROP_AUTOSIZE, CV_WINDOW_NORMAL);
		cvSetWindowProperty("Detected Blob", CV_WND_PROP_ASPECTRATIO, CV_WINDOW_KEEPRATIO);
		cvResizeWindow("Detected Blob", 1400, 800);
		stream1.set(CV_CAP_PROP_POS_MSEC, 90000);
		while (true) 
		{
			int totalTime =0;
			QTime ti;
			ti.start();

			clock_t t = clock();
			if (!(stream1.read(frame)))
				break;

			vector< vector< Point> > contours;
			BlobDetection blbDetect;
			resize(frame, frame, Size(frame.size().width, frame.size().height));

			Mat originalFrame = frame.clone();

			contours = blbDetect.detectContours(frame, pMOG2Pointer, fgMaskMOG2);


			Rect roi;
			vector<vector<Point> >hulls;
			vector< vector< Point> > filteredContours;

			vector<Point> cutoffRegion;
			//int constantval = 300;
			cutoffRegion.push_back(Point(0, 0));
			cutoffRegion.push_back(Point(170, 0));
			cutoffRegion.push_back(Point(180, 290));
			cutoffRegion.push_back(Point(110,450));
			cutoffRegion.push_back(Point(0, 480));
			vector<vector<Point>> blobsInCutoff;
			while (contours.size() != 0)
			{
				vector<Point> contour = contours[contours.size() - 1];
				
				if (blbDetect.isQualifyingContour(contour, cutoffRegion, &blobsInCutoff))
				{
					vector<Point> tempHull;
					convexHull(Mat(contour), tempHull, false);
					hulls.push_back(tempHull);
					filteredContours.push_back(contour);
				}
				contours.pop_back();
			}

			//Print Current Time in Frame
			int time = static_cast<int>(stream1.get(CV_CAP_PROP_POS_MSEC));
			int mins = static_cast<int>(time / (1000 * 60));
			int seconds = static_cast<int>((time - (mins * 60 * 1000)) / 1000);
			string timeStr;
			if (seconds < 10)
			{
				timeStr = to_string(mins) + ".0" + to_string(seconds);
			}
			else
			{
				timeStr = to_string(mins) + "." + to_string(seconds);
			}



			vector<BlobId> profiledBlobs;
			if (filteredContours.size() != 0){
				//profiledBlobs = blb.matchProfilesWithBlobs(filteredContours, timeStr, file,mysq);

			}

			//Draw the hull borders and fill in white to create the "hullDrawing" mask 
			Mat drawnOnOriginal = originalFrame.clone();
			Mat hullDrawing(frame.size(), CV_8UC3);
			floodFill(hullDrawing, Point(), Scalar(255, 255, 255));
			//Draw cutoff

			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			//for (int i = 0; i < cutoffRegion.size()-1; i++)
			//{
			//	line(drawnOnOriginal, cutoffRegion[i], cutoffRegion[i + 1], color, 2);
			//}
			

			for (int i = 0; i < filteredContours.size(); i++)
			{
				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
				drawContours(hullDrawing, hulls, i, color, 1, 8, vector<Vec4i>(), 0, Point());
				drawContours(drawnOnOriginal, hulls, i, color, 1, 8, vector<Vec4i>(), 0, Point());
			}
			for (int i = 0; i < profiledBlobs.size(); i++)
			{
				BlobId profiledBlob = profiledBlobs[i];
				Point textOrg(profiledBlob.centreX, profiledBlob.centreY);
				putText(drawnOnOriginal, profiledBlob.Id, textOrg, FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 0), 1, CV_AA);
			}
			vector< vector< Point> >::iterator itc = filteredContours.begin();
			while (itc != filteredContours.end()) {
				Rect mr = boundingRect(Mat(*itc));
				rectangle(drawnOnOriginal, mr, CV_RGB(255, 0, 0), 3);
				++itc;
			}

			//int size = blobsInCutoff.size();
			vector< vector< Point> >::iterator itc2 = blobsInCutoff.begin();
			while (itc2 != blobsInCutoff.end()) {
				Rect mr = boundingRect(Mat(*itc2));
				rectangle(drawnOnOriginal, mr, CV_RGB(0, 255, 0), 3);
				Point textOrg(mr.x + mr.width/2, mr.y + mr.height/2);
				putText(drawnOnOriginal, "OUT", textOrg, FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 0), 1, CV_AA);
				++itc2;
			}

			floodFill(hullDrawing, Point(), Scalar(0, 0, 0));
			putText(drawnOnOriginal, timeStr, cvPoint(30, 30),
				FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0, 0, 0), 2, CV_AA);


			imshow("Detected Blob", drawnOnOriginal);


			cvWaitKey(600000);
		
			
		}
	}
	return 0;
}

Mat drawHist(Mat greyImage)
{////
	float colourFrequency2[256] = {}; 
	for (int i = 0; i < greyImage.rows; i++){
		for (int j = 0; j < greyImage.cols; j++){
			int pixel = greyImage.at<uchar>(i, j);
			colourFrequency2[pixel] += 1;
		}
	}

	float max = 0.0;
	for (int i = 0; i < 256; i++){
		if (max < colourFrequency2[i]){
			max = colourFrequency2[i];
		}
	}
	Mat histogram2(310, 260, CV_8UC3, Scalar(255, 255, 255));

	int hist2[256] = {};
	float maxd = max;
	for (int i = 0; i < 256; i++){
		hist2[i] = cvRound(double(colourFrequency2[i] / maxd) * 300.0);
		Point pt1 = Point(i, 300 - hist2[i]);
		Point pt2 = Point(i, 300);
		line(histogram2, pt1, pt2, Scalar(0, 0, 0), 1, 8, 0);
	}

	return histogram2;

}