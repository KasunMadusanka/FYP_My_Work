#include "VideoProcessing.h"

#define RWIDTH 320 //640 //800
#define RHEIGHT 240 //480 //600

VideoProcessing::VideoProcessing(){}

int VideoProcessing::blobDetection(Mat frame, Ptr<BackgroundSubtractor> pMOG2, Mat mask, vector<models::Blob> *outBlobs, vector<Point> cutOffRegions, vector<vector<Point>>* blobsInCutoff)
{
	vector<vector<Point>> contours;
	contours = blbDetect.detectContours(frame, pMOG2, mask);
	if (cutOffRegions.size() == 0)
	{
		for each (vector<Point> con in contours)
		{
			if (blbDetect.isQualifyingContour(con))
				(*outBlobs).push_back(models::Blob(con));
		}
	}
	else
	{
		for each (vector<Point> con in contours)
		{
			if (blbDetect.isQualifyingContour(con, cutOffRegions, blobsInCutoff))
				(*outBlobs).push_back(models::Blob(con));
		}
	}
	/*if (outBlobs->size() > 0)
		__debugbreak();*/

	return (*outBlobs).size();
}

//int VideoProcessing::GPU_BlobDetection(Mat frame, Ptr<BackgroundSubtractor> pMOG2, Mat mask, vector<models::Blob> *outBlobs, vector<Point> cutOffRegions, vector<vector<Point>>* blobsInCutoff)
//{
//	vector<vector<Point>> contours;
//	gpu::GpuMat o_frame_gpu;
//
//	//scale and check processing time
//	unsigned long AAtime = 0, BBtime = 0;
//	float scaleX = float(frame.size().width) / RWIDTH;
//	float scaleY = float(frame.size().height) / RHEIGHT;
//
//	contours = blbDetect.GPU_DetectContours(frame, o_frame_gpu);
//
//	//Scale 
//	if (cutOffRegions.size() == 0)
//	{
//		for each (vector<Point> con in contours)
//		{
//			if (blbDetect.isQualifyingContour(con))
//				(*outBlobs).push_back(models::Blob(con));
//		}
//	}
//	else
//	{
//		for each (vector<Point> con in contours) 
//		{
//			if (blbDetect.isQualifyingContour(con, cutOffRegions, blobsInCutoff))
//				(*outBlobs).push_back(models::Blob(con));
//		}
//	}
//
//	return (*outBlobs).size();
//}


int VideoProcessing::humanDetection(vector<models::Blob> *blobs, Mat *frame, vector<models::HumanBlob> *outHumanBlobs, VideoCapture *cap, string link, SVM__Class* svmPointer, Connection* mySqlConnection)
{


	// uncomment for senior dataset ///////////////////////////////////////
	/*for (vector<models::Blob>::iterator i = blobs->begin(); i != blobs->end(); i++)
	{
		models::HumanBlob hb = models::HumanBlob(*i);
		hb.profileID = "NULL";
		outHumanBlobs->push_back(hb);
	}
	return outHumanBlobs->size();*/



	vector<vector<Point>> blobContourVector;
	
	//if (link == "C:/AdaptiveCameraNetworkPack/Videos/PRG6.avi"){
	//	for (vector<models::Blob>::iterator it = blobs->begin(); it != blobs->end(); it++)
	//	{
	//		Rect roi = boundingRect(it->getContour());
	//		Mat rectBlob = (*frame)(roi);
	//		Mat rectBlobClone = rectBlob.clone();
	//		//namedWindow("HUMAN", CV_WINDOW_NORMAL);
	//		//namedWindow("NON HUMAN", CV_WINDOW_NORMAL);
	//		//cvWaitKey(1);
	//		bool temp = svmPointer->predict_SingleSVMfromMat(rectBlob);
	//		if (temp)
	//		{
	//			//imshow("HUMAN", rectBlobClone);
	//			blobContourVector.push_back(it->getContour());
	//		}
	//		else
	//		{
	//			//imshow("NON HUMAN", rectBlobClone);
	//		}
	//	}
	//}
	//else
	//{
		for (vector<models::Blob>::iterator it = blobs->begin(); it != blobs->end(); it++)
		{
			blobContourVector.push_back(it->getContour());
		}
	//}
	BlobDetection blbDetection;
	int time = static_cast<int>(cap->get(CV_CAP_PROP_POS_MSEC));
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
	stringstream ss(link); 
	string item;
	vector<string> tokens;
	while (getline(ss, item, '/')) {
		tokens.push_back(item);
	}
	vector<BlobId> profiledBlobs = blbDetection.matchProfilesWithBlobs(blobContourVector, timeStr, tokens[tokens.size() - 1],mySqlConnection);
	for (int i = 0; i < profiledBlobs.size(); i++)
	{

		models::HumanBlob hb = models::HumanBlob(models::Blob(blobContourVector[i]));
		hb.profileID = profiledBlobs[i].Id;

		//Recording profile hit in db
		ProfileHits* pLogger = new ProfileHits(mySqlConnection);
		pLogger->profileLog(link, profiledBlobs[i].Id, timeStr);
		delete pLogger;
		pLogger = NULL;

		outHumanBlobs->push_back(hb);

	}

	/*if (outHumanBlobs->size() > 0)
		__debugbreak();*/
	return outHumanBlobs->size();
}


void VideoProcessing::dataAssociation(
	vector<models::Blob> *blobs,
	vector<models::HumanBlob> *trackingHumanBlobs,
	vector<models::Blob> *outUnidentifiedBlobs,
	vector<models::MissingHumanBlob> *outMissingHumanBlobs)
{
	vector<Mat> Predicted;
	vector<Point2f> Detected;
	vector<vector<int>> CostMetrix;

	for (vector<models::Blob>::iterator i = blobs->begin(); i != blobs->end(); i++)
	{
		outUnidentifiedBlobs->push_back(*i);
	}

}

void VideoProcessing::checkInProfiles(
	vector<models::HumanBlob> *humanList,
	vector<models::HumanBlob> *possibleList,
	vector<models::MissingHumanBlob> *missingList,
	vector<models::HumanBlob> *trackingList)
{
	// first check in possible profile list if not find in missing list
	// update the tracking list and possible list and missing list
	// send unidentified human list

	// by using feature vectors

	for (vector<models::HumanBlob>::iterator h = humanList->begin(); h != humanList->end(); h++)
	{
		for (vector<models::HumanBlob>::iterator p = possibleList->begin(); p != possibleList->end(); p++)
		{
			if ((*h) == (*p))
			{
				trackingList->push_back(models::HumanBlob(*p));
				possibleList->erase(p);
				humanList->erase(h);
				// send message to main profile set to flush from others
			}
		}
	}
	for (vector<models::HumanBlob>::iterator h = humanList->begin(); h != humanList->end(); h++)
	{
		for (vector<models::MissingHumanBlob>::iterator m = missingList->begin(); m != missingList->end(); m++)
		{
			if ((*h) == m->humanBlob)
			{
				trackingList->push_back(models::MissingHumanBlob(*m).humanBlob);
				missingList->erase(m);
				humanList->erase(h);
			}
		}
	}

	// update missing list
	for (vector<models::MissingHumanBlob>::iterator m = missingList->begin(); m != missingList->end(); m++)
	{
		m->missedTime++;
		if (m->missedTime > 10)
		{
			// give warning to main that one missed for long time
			missingList->erase(m);
		}
	}

}

void VideoProcessing::initTrackingObject(vector<models::HumanBlob> *humanList, vector<models::HumanBlob> *trackingList)
{
	// create a central profile
	// set profile id to human blob and push back to tracking blob
	trackingList->clear();
	for (vector<models::HumanBlob>::iterator i = humanList->begin(); i != humanList->end(); i++)
	{
		trackingList->push_back(*i);
	}
}

void VideoProcessing::kalmanCorrectAndPredict(vector<models::HumanBlob> *trackingList)
{
	Mat_<float> mesurement(2, 1);
	mesurement.setTo(Scalar(0));
	Point temp;
	for (size_t i = 0; i < trackingList->size(); i++)
	{
		// Kalman Correct
		temp = trackingList->at(i).centerPointList.back();
		mesurement(0) = temp.x;
		mesurement(1) = temp.y;
		trackingList->at(i).kalmanFilter.correct(mesurement);
		mesurement.empty();

		// Kalman Predict
		trackingList->at(i).kalmanFilter.predict();
	}
}

void VideoProcessing::informAdjecentNodes(vector<graph::ExitPoint> *exitsList, vector<models::HumanBlob> *trackingList)
{
	
}

VideoProcessing::~VideoProcessing(){}