#include "threadfornode.h"
#include "messagepasser.h"

ThreadForNode::ThreadForNode()
{
	msgPasser = new MessagePasser();
	acknowledged = true;
	connect(
		this, SIGNAL(sendProfileToNode(ProfileTransferObj, ThreadForNode*)), 
		msgPasser, SLOT(passMessage(ProfileTransferObj, ThreadForNode*))
		);
}

ThreadForNode::~ThreadForNode()
{

}

void ThreadForNode::run()
{

	RNG rng(12345);
	VideoCapture cap(videoLink);
	
	//vector<graph::ExitPoint> exitPoints = currentNodePtr->exitPoints;
	vector<models::Blob> blobs, unidentifiedBlobs;
	vector<HumanBlob> humanBlobs, trackingHumanBlobs, possibleProfileList;
	vector<MissingHumanBlob> missingHumanBlobs;

	//gpu::GpuMat Mog_Mask_g, Mog_MaskMorpho_g;

	VideoProcessing _vProcessing = VideoProcessing();
	Mat fgMaskMOG2;
	Ptr<BackgroundSubtractor> pMOG2 = new BackgroundSubtractorMOG2(300, 32, true);
	bool ended = false;

	if (!cap.isOpened())
	{
		qDebug() << "Error loding video '" << QString::fromStdString(videoLink) << "'";
	}
	//cap.set(CV_CAP_PROP_POS_FRAMES, 10507);
	//int constantval = 300;
	Scalar cutOffRegionCol = Scalar(0, 255, 0);
	while (cap.read(frame))
	{
		if (ended)
			break;

		if (cap.get(CV_CAP_PROP_POS_FRAMES) == cap.get(CV_CAP_PROP_FRAME_COUNT))
			ended = true;

		cvtColor(frame, frame, CV_BGR2RGB);

		frameToBeRaped = frame.clone();
		// ////////////////// //
		// process begin here //
		// ////////////////// //

		blobs.clear();
		unidentifiedBlobs.clear();
		humanBlobs.clear();
		blobsInCutoff.clear();

		string x = this->nodeId;

		vector<Point> scaledCutoffRegions;
		resizeContour(cutoffRegion, 1.0/2.200005, 1.0/2.00000, &scaledCutoffRegions);
		/*if (nodeId == "C008")
			__debugbreak();*/
		if (cutoffRegion.size() != 0)
		{
			for (int i = 0; i < cutoffRegion.size() - 1; i++)
			{
				line(frame, cutoffRegion[i], cutoffRegion[i + 1], cutOffRegionCol, 8);
			}
		}
		// blob detection
		if (_vProcessing.blobDetection(frameToBeRaped, pMOG2, fgMaskMOG2, &blobs, scaledCutoffRegions, &blobsInCutoff) == 0)
		{
			if (blobsInCutoff.size() != 0)
			{
				drawBlobsAndWriteInFrame(frame, &blobsInCutoff, cutOffRegionCol, "OUT", Scalar(255, 0, 0));
			}
			
			QImage outImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
			outImage = outImage.scaled(this->releventUiLable->width(), this->releventUiLable->height(), Qt::KeepAspectRatio);
			QPainter qpainter(&outImage);
			qpainter.setBrush(Qt::NoBrush);
			qpainter.setPen(Qt::red);
			qpainter.setFont(QFont("Times", 12, QFont::Normal));
			qpainter.drawText(outImage.rect(), Qt::AlignTop, QString::fromStdString(to_string(cap.get(CV_CAP_PROP_POS_FRAMES))));

			emit sendFrameToMain(outImage, this);
			this->acknowledged = false;

			waitForAcknowledge();

			continue;	// If no blobs detected continue while
		}

		// scalling for gpu outputs
		vector<Point> temp;
		for (vector<models::Blob>::iterator i = blobs.begin(); i != blobs.end(); i++)
		{
			resizeContour(i->getContour(), 2.200005, 2.000000, &temp); // camnet dataset
			//resizeContour(i->getContour(), 1.100005, 1.200000, &temp); // senior data set
			i->setContour(temp);
			temp.clear();
		}
		//mockFunction(&blobs, &trackingHumanBlobs, &cap);

		if (trackingHumanBlobs.empty()) // blobs >>> unindentified
		{
			unidentifiedBlobs = blobs;
		}
		else
		{
			_vProcessing.dataAssociation(&blobs, &trackingHumanBlobs, &unidentifiedBlobs, &missingHumanBlobs);
		}
		
		if (!(unidentifiedBlobs.empty())) // unindentified >>> human
		{
			// done through hardcoded function
			_vProcessing.humanDetection(&unidentifiedBlobs, &frameToBeRaped, &humanBlobs, &cap, this->videoLink, svmPointer);
		}
		
		if (!(humanBlobs.empty())) // human >>> trackinghuman
		{
			//_vProcessing.checkInProfiles(&humanBlobs, &possibleProfileList, &missingHumanBlobs, &trackingHumanBlobs);
		}
		
		if (!(humanBlobs.empty()))
		{
			_vProcessing.initTrackingObject(&humanBlobs, &trackingHumanBlobs);
		}
		
		if (!(trackingHumanBlobs.empty()))
		{
			_vProcessing.kalmanCorrectAndPredict(&trackingHumanBlobs);
			//_vProcessing.informAdjecentNodes(&exitPoints, &trackingHumanBlobs);
			//_vProcessing.UpdateCentralProfiles(&trackingHumanBlobs);
		}







		//// get the moments
		//vector<Moments> mu(human_blobs.size());
		//for (size_t i = 0; i < human_blobs.size(); i++)
		//{
		//	mu[i] = moments(human_blobs[i], false);
		//}

		//// get the mass centers:
		//vector<Point2f> mc(human_blobs.size());
		//for (size_t i = 0; i < human_blobs.size(); i++)
		//{
		//	mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		//}

		//for (size_t i = 0; i < mc.size(); i++)
		//{
		//	int selectedContourId = getReleventContour(mc[i]);

		//	Point measurePre(trackingObjectMap[selectedContourId].getLastPoint());
		//	vector<Point> pointsVector = trackingObjectMap[selectedContourId].addCenterPoint(mc[i]);
		//	Point measureCurrent(trackingObjectMap[selectedContourId].getLastPoint());

		//	drawCross(frame, measureCurrent, Scalar(0, 255, 0), 5);

		//	Mat prediction = trackingObjectMap[selectedContourId].kalmanPredict();
		//	Point kalmanPredict(prediction.at<float>(0), prediction.at<float>(1));

		//	Mat corrected = trackingObjectMap[selectedContourId].kalmanCorrect(mc[i]);
		//	Point kalmanCorrected(corrected.at<float>(0), corrected.at<float>(1));
		//	
		//	drawCross(frame, kalmanPredict, Scalar(0, 0, 255), 5);
		//	//drawCross(frame, kalmanCorrected, Scalar(0, 255, 255), 5);

		//	stringstream lbl;
		//	lbl << "Blob_" << selectedContourId;
		//	putText(frame, lbl.str(), measureCurrent, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 255), 2);

		//	for (int i = pointsVector.size() - 1; i != 0; i--)
		//	{
		//		line(frame, pointsVector[i], pointsVector[i - 1], Scalar(200, 220, 80), 3);
		//	}
		//}


		
		
		
		QImage outImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
		outImage = outImage.scaled(this->releventUiLable->width(), this->releventUiLable->height(), Qt::KeepAspectRatio);
		QPainter qpainter(&outImage);
		qpainter.setBrush(Qt::NoBrush);
		qpainter.setPen(Qt::red);
		qpainter.setFont(QFont("Times", 12, QFont::Normal));

		qpainter.drawText(outImage.rect(), Qt::AlignTop, QString::fromStdString(to_string(cap.get(CV_CAP_PROP_POS_FRAMES))));

		double sx = (double)outImage.width() / (double)frame.size().width;
		double sy = (double)outImage.height() / (double)frame.size().height;
		vector<Point> cnt;
		Point rectStart;
		Point rectEnd;


		// change blob into TrackingHumanBlob when code is done //
		for (vector<models::HumanBlob>::iterator i = trackingHumanBlobs.begin(); i != trackingHumanBlobs.end(); i++)
		{
			resizeContour(i->blob.getContour(), sx, sy, &cnt);
			Rect br = boundingRect(cnt);
			rectStart = Point(br.x, br.y);
			rectEnd = Point(br.width, br.height);
			qpainter.setPen(Qt::yellow);
			qpainter.setFont(QFont("Times", 8, QFont::StyleItalic));
			qpainter.drawRect(rectStart.x, rectStart.y, rectEnd.x, rectEnd.y);
			qpainter.drawText(QRect(rectStart.x, rectStart.y, rectEnd.x, rectEnd.y), Qt::AlignTop, QString::fromStdString(i->profileID));
		}

		
		emit sendFrameToMain(outImage, this);
		this->acknowledged = false;
		waitForAcknowledge();
	}

	QImage finalImage(this->releventUiLable->width(), this->releventUiLable->width(), QImage::Format_RGB888);
	QPainter qp(&finalImage);
	qp.setBrush(Qt::black);
	qp.setPen(Qt::red);
	qp.setFont(QFont("Times", 12, QFont::Bold));
	qp.drawText(finalImage.rect(), Qt::AlignCenter, "END OF FILE!");

	emit sendFrameToMain(finalImage, this);
	emit sendFinishedToMain();

	waitForAcknowledge();
	
	qDebug() << "finished.";
}

void ThreadForNode::updateProfileList(ProfileTransferObj profile)
{

}


void ThreadForNode::waitForAcknowledge()
{
	mutex->lock();
	isNotShown->wait(mutex);
	mutex->unlock();
	/*if (!acknowledged)
	{
		mutex->lock();
			isNotShown->wait(mutex);
		mutex->unlock();
	}*/
	//while (!acknowledged);

}

void ThreadForNode::resizeContour(vector<Point> contour, double xScalar, double yScalar, vector<Point>* cnt)
{
	cnt->clear();
	for (int i = 0; i < contour.size(); i++)
	{
		Point pnt;
		pnt.x = contour[i].x * xScalar;
		pnt.y = contour[i].y * yScalar;
		cnt->push_back(pnt);
	}
}

void ThreadForNode::drawBlobsAndWriteInFrame(Mat frame, vector<vector<Point>>* blobs, Scalar color,string writing, Scalar writingColor)
{
	vector< vector< Point> >::iterator itc2 = blobs->begin();
	while (itc2 != blobs->end()) {
		Rect mr = boundingRect(Mat(*itc2));
		rectangle(frame, mr, color, 3);
		Point textOrg(mr.x + mr.width / 2, mr.y + mr.height / 2);
		putText(frame, writing, textOrg, FONT_HERSHEY_COMPLEX_SMALL, 0.7, writingColor, 1, CV_AA);
		++itc2;
	}

}

void ThreadForNode::mockFunction(vector<models::Blob> *blobs, vector<models::HumanBlob> *trackingHumanBlobs, VideoCapture *cap)
{
	trackingHumanBlobs->clear();
	vector<vector<Point>> blobContourVector;
	for (vector<models::Blob>::iterator it = blobs->begin(); it != blobs->end(); it++)
	{
		blobContourVector.push_back(it->getContour());
	}
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
	stringstream ss(this->videoLink);
	string item;
	vector<string> tokens;
	while (getline(ss, item, '/')) {
		tokens.push_back(item);
	}
	vector<BlobId> profiledBlobs = blbDetection.matchProfilesWithBlobs(blobContourVector, timeStr, tokens[tokens.size()-1]);
	for (int i = 0; i < profiledBlobs.size(); i++)
	{
		HumanBlob hb = HumanBlob(blobs->at(i));
		hb.profileID = profiledBlobs[i].Id;
		trackingHumanBlobs->push_back(hb);
	}
}