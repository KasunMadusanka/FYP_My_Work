#ifndef THREADFORNODE_H
#define THREADFORNODE_H

//#include <QObject>
#include "QtCore\qobject.h"
#include "QtCore\qthread.h"
#include "QtGui\qimage.h"
#include "QtGui\qpainter.h"
#include "QtCore\qdebug.h"

#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\video\tracking.hpp>
#include <opencv2\video\background_segm.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "VideoProcessing.h"
#include "ui_qttesting.h"

#include "QtCore\qmutex.h"
#include "QtCore\qwaitcondition.h"
#include "svm__class.h"

using namespace models;
using namespace cv;

class MessagePasser;

class ThreadForNode : public QThread
{
	Q_OBJECT

public:
	ThreadForNode();
	~ThreadForNode();
	void run();
	void updateProfileList(ProfileTransferObj profile);
	void waitForAcknowledge();
	void mockFunction(vector<models::Blob> *blobs, vector<models::HumanBlob> *trackingHumanBlobs, VideoCapture *cap);

	SVM__Class* svmPointer;
	string videoLink; // temp
	string nodeId;
	vector<graph::ExitPoint> exitPoints;
	bool acknowledged;
	QWaitCondition* isNotShown;
	QMutex* mutex;
	int startFrame;
	QLabel* releventUiLable;
	vector<Point> cutoffRegion;
	vector<vector<Point>> blobsInCutoff;
	Connection* mysqlConnection;

signals:
	void sendFrameToMain(QImage outImage, ThreadForNode* thread);
	void sendProfileToNode(ProfileTransferObj profile, ThreadForNode* nodeThread);
	void sendFinishedToMain();

private:
	void resizeContour(vector<Point> contour, double xScalar, double yScalar, vector<Point>* cnt);
	void drawBlobsAndWriteInFrame(Mat frame, vector<vector<Point>>* blobs, Scalar color, string writing, Scalar writingColor);
	MessagePasser* msgPasser;
	Mat frame;
	Mat frameToBeRaped;
};

#endif // THREADFORNODE_H
