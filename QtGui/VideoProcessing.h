#include <opencv2\highgui\highgui.hpp>	// open cv
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\tracking.hpp>
#include <opencv2\video\background_segm.hpp>
#include "Models.h"
#include "graph.h"
#include "BlobDetection.h"
#include "svm__class.h"
#include "Logger.h"
#include <math.h>

// blob detection
// in	: *frame
// out	: contours

// human detection
// in	: *contours, *frame
// out	: filtered_contours

// kalman filtering

// comparison algo (get relevent contours)

using namespace std;
using namespace cv;


class VideoProcessing
{
public:
	VideoProcessing();
	int blobDetection(Mat frame, Ptr<BackgroundSubtractor> pMOG2, Mat mask, vector<RowBlob> *outBlobs, vector<Point> cutOffRegions, vector<vector<Point>>* blobsInCutoff);
	//int GPU_BlobDetection(Mat frame, Ptr<BackgroundSubtractor> pMOG2, Mat mask, vector<RowBlob> *outBlobs, vector<Point> cutOffRegions, vector<vector<Point>>* blobsInCutoff);
	int humanDetection(vector<RowBlob> *blobs, Mat *frame, vector<HumanBlob> *outHumanBlobs, VideoCapture *cap, string link, SVM__Class* svmPointer,  Connection* mySqlConnection);

	void dataAssociation(
		vector<HumanBlob> *humanBlobs,
		vector<HumanBlob> *trackingHumanBlobs,
		vector<MissingHumanBlob> *outMissingHumanBlobs);

	void checkInProfiles(
		vector<HumanBlob> *humanList,
		vector<HumanBlob> *possibleList,
		vector<MissingHumanBlob> *missingList,
		vector<HumanBlob> *trackingList);

	void initTrackingObject(vector<HumanBlob> *humanList, vector<HumanBlob> *trackingList);

	void kalmanCorrectAndPredict(vector<HumanBlob> *trackingList);

	~VideoProcessing();

private:
	Mat morpho_ele = getStructuringElement(MORPH_RECT, Size(7, 7), Point(3, 3));
	BlobDetection blbDetect;
protected:

};