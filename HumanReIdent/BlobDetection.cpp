#include "stdafx.h"

#include "humanreident.h"

#include "BlobDetection.h"
#include < time.h>

#define RWIDTH 320 //640 //800
#define RHEIGHT 240 //480 //600

BlobDetection::BlobDetection()
{
	//pMOG2_g = gpu::MOG2_GPU(30);
	//pMOG2_g.history = 300; //300;
	//pMOG2_g.varThreshold = 15; //64; //128; //64; //32;//; 
	//pMOG2_g.bShadowDetection = true; // true;//
	//
	//gpurgb2 = vector< gpu::GpuMat>(3);
	//gpurgb = vector< gpu::GpuMat>(3);

	element = getStructuringElement(MORPH_RECT, Size(9, 9), Point(4, 4));
}


BlobDetection::~BlobDetection()
{
}

vector< vector< Point> > BlobDetection::detectContours(Mat frame, Ptr< BackgroundSubtractor>& pMOG2Pointer , Mat& fgMaskMOG2)
{
	vector< vector< Point> > result;

	//cvNamedWindow("Original"	, CV_WINDOW_NORMAL);
	//cvNamedWindow("Blurred"		, CV_WINDOW_NORMAL);
	////cvNamedWindow("fgMaskMOG2X"	, CV_WINDOW_NORMAL);
	//cvNamedWindow("Background Subtracted", CV_WINDOW_NORMAL);
	//cvNamedWindow("Shadow Removed"	, CV_WINDOW_NORMAL);

	Mat fgMaskMOG2X = fgMaskMOG2.clone(); 

	Mat ContourImg; 
	Ptr< BackgroundSubtractor> pMOG2 = pMOG2Pointer; 
	Mat element = getStructuringElement(MORPH_RECT, Size(7, 7), Point(3, 3));
	//imshow("Original", frame);

	//PreProcess
	blur(frame, frame, Size(4, 4));
	//imshow("Blurred", frame);

	//Background subtraction
	pMOG2->operator()(frame, fgMaskMOG2X, -1);
	//imshow("fgMaskMOG2X", frame);

	morphologyEx(fgMaskMOG2X, frame, CV_MOP_CLOSE, element);
	//imshow("Background Subtracted", frame);

	threshold(frame, frame, 180, 255, CV_THRESH_BINARY);
	//imshow("Shadow Removed", frame);

	cvWaitKey(1);
	ContourImg = frame.clone();
	findContours(ContourImg,
		result, // a vector of contours
		CV_RETR_EXTERNAL, // retrieve the external contours
		CV_CHAIN_APPROX_NONE); // all pixels of each contours


	fgMaskMOG2 = fgMaskMOG2X.clone();
	return result;
}

//vector<vector<Point>> BlobDetection::GPU_DetectContours(Mat o_frame, gpu::GpuMat o_frame_gpu)
//{
//
//	//scale and check processing time
//	float scaleX = float(o_frame.size().width) / RWIDTH;
//	float scaleY = float(o_frame.size().height) / RHEIGHT;
//
//	unsigned long AAtime = 0, BBtime = 0;
//	const clock_t begin_time = clock(); // by niro
//
//	//get frame and upload to gpu
//	o_frame_gpu.upload(o_frame);
//
//	//resize
//	gpu::resize(o_frame_gpu, r_frame_gpu, Size(RWIDTH, RHEIGHT)); //for blob labeling
//	//AAtime = getTickCount();
//	
//	//blur red blue and green channels
//	gpu::split(r_frame_gpu, gpurgb);
//	gpu::blur(gpurgb[0], gpurgb2[0], Size(3, 3));
//	gpu::blur(gpurgb[1], gpurgb2[1], Size(3, 3));
//	gpu::blur(gpurgb[2], gpurgb2[2], Size(3, 3));
//	gpu::merge(gpurgb2, r_blur_gpu);
//	//mog
//	pMOG2_g.operator()(r_blur_gpu, Mog_Mask_g, -1);
//	Mog_Mask_g.download(Mog_Mask);
//	//mopnology
//	//gpu::morphologyEx(Mog_Mask_g, Mog_MaskMorpho_g, CV_MOP_CLOSE, element);
//	gpu::morphologyEx(Mog_Mask_g, Mog_MaskMorpho_g, CV_MOP_DILATE, element);
//	//binary
//	gpu::threshold(Mog_MaskMorpho_g, r_binary_gpu, 128, 255, CV_THRESH_BINARY);
//	
//	
//	//Blob Labeling
//	//Find contour   
//	Mat ContourImg;
//	r_binary_gpu.download(ContourImg);
//	//less blob delete   
//	vector< vector< Point> > contours;
//	findContours(ContourImg,
//		contours, // a vector of contours   
//		CV_RETR_EXTERNAL, // retrieve the external contours   
//		CV_CHAIN_APPROX_NONE); // all pixels of each contours  
//	
//	//processing time print
//	//BBtime = getTickCount();
//	//float pt = (BBtime - AAtime) / getTickFrequency();
//	//float fpt = 1 / pt;
//	//printf("gpu %.4lf / %.4lf \n", pt, fpt);
//	float time = float(clock() - begin_time) / CLOCKS_PER_SEC;
//	qDebug() << "Back ground Substraction Time in GPU" << time << " ms";// by niro
//	
//	return contours;
//
//
//}

bool BlobDetection::isQualifyingContour(vector<Point> contour)
{ 
	bool result = false;
	int minimum_width = 30;//30;
	int maximum_width = 180;//100;
	int minimum_height = 30;// 30;
	double minimum_htow_ratio = 1.3;
	Rect roi = boundingRect(contour);
	double heightToWidthRatio = static_cast<double>(roi.height) / static_cast<double>(roi.width);
	if (	roi.width			>	minimum_width	&&
			roi.width			<	maximum_width	&&
			roi.height			>	minimum_height	&&
			heightToWidthRatio	>	minimum_htow_ratio
		)
	{
		result = true;
	}

	//second test
	// pointPolygonTest(InputArray contour, Point2f pt, bool measureDist)


	return result;
	

}
bool BlobDetection::isQualifyingContour(vector<Point> contour, vector<Point>cutOffRegion, vector<vector<Point>> *blobsInCutoff)
{

	bool ratioQualified = false;
	int minimum_width = 30;//30;
	int maximum_width = 180;//100;
	int minimum_height = 30;// 30;
	double minimum_htow_ratio = 1.3;
	Rect roi = boundingRect(contour);
	double heightToWidthRatio = static_cast<double>(roi.height) / static_cast<double>(roi.width);
	if (roi.width			>	minimum_width	&&
		roi.width			<	maximum_width	&&
		roi.height			>	minimum_height	&&
		heightToWidthRatio	>	minimum_htow_ratio
		)
	{
		ratioQualified = true;
	}

	if (ratioQualified)
	{
		bool withinCutOff = true;
		vector<Point> boundinBox;
		//int constantval = 300;
		boundinBox.push_back(Point(roi.x, roi.y));
		boundinBox.push_back(Point(roi.x + roi.width, roi.y));
		boundinBox.push_back(Point(roi.x + roi.width, roi.y + roi.width));
		boundinBox.push_back(Point(roi.x , roi.y + roi.width));

		for (int pointId = 0; pointId < boundinBox.size(); pointId++)
		{
			Point pt = boundinBox[pointId];
			int withinCutOffD = pointPolygonTest(cutOffRegion, pt, true);
			if (withinCutOffD<0)
			{
				//if at least one point is not within the cutoff region this becomes a qualifying blob
				return true;
			}
		}
		//if this point is reached the whole contour is withing the cutoff
	
		blobsInCutoff->push_back(contour);
		return false;

	}
	//This is reached if ratioQualified is false
	return ratioQualified;

}


vector<BlobId> BlobDetection::matchProfilesWithBlobs(vector< vector< Point> > contours, string absoluteTime, string cameraNode, Connection* mysqlConnection)
{
	HumanHits* hh = new HumanHits(mysqlConnection);
	vector<BlobId> profiledBlobs;
	vector<Profile> profilesExisting = hh->getAllProfilesInSecond(absoluteTime, cameraNode);

	//Start comparing blob with existing profile
	int counter = 0;
	for (vector< Point> contour : contours)
	{
		//Find centre of blob
		Moments mom = moments(contour, false);
		Point2f currentCentrePoint = Point2f(mom.m10 / mom.m00, mom.m01 / mom.m00);

		//Map blob to profile, if not found set to UNKNOWN
		double minDistance = -1;
		string minProfile = "UNKNOWN";
		for (int profileCount = 0; profileCount < profilesExisting.size(); profileCount++)
		{
			Profile savedProfile = profilesExisting[profileCount];
			

			double distance = sqrt(
				(currentCentrePoint.x - savedProfile.centreX)*(currentCentrePoint.x - savedProfile.centreX)
				+
				(currentCentrePoint.y - savedProfile.centreY)*(currentCentrePoint.y - savedProfile.centreY)
				);

		
			if (abs(currentCentrePoint.x - savedProfile.centreX) > 120 || abs(currentCentrePoint.y - savedProfile.centreY) > 100)
			{
				continue;
			}
			if (minDistance = -1 || minDistance > distance)
			{
				minDistance = distance;
				minProfile = savedProfile.profileId;
				profilesExisting.erase(profilesExisting.begin() + profileCount);
			}

		}
		BlobId blobId;
		blobId.Id = minProfile;
		blobId.surroundingContours = contour;
		blobId.centreX = currentCentrePoint.x;
		blobId.centreY = currentCentrePoint.y;
		profiledBlobs.push_back(blobId);
		counter++;
	}


	delete hh;
	hh = NULL;
	return profiledBlobs;
}


vector< vector< Point> > BlobDetection::detectEnclosingConvexHull(vector<vector<Point>> contours)
{
	vector< vector< Point> > hull;
	return hull;
}