#include "threadfornode.h"

ThreadForNode::ThreadForNode()
{
	acknowledged = true;
	connect(this, SIGNAL(flushProfile(QString)),
		this, SLOT(flushAllPossible(QString)));
	connect(this, SIGNAL(receiveProfile(HumanBlob*)),
		this, SLOT(updateProfileList(HumanBlob*)));
}

ThreadForNode::~ThreadForNode()
{

}

void ThreadForNode::flushAllPossible(QString pId)
{
	for (vector<HumanBlob>::iterator it = possibleProfileList.begin(); it != possibleProfileList.end(); it++)
	{
		if (it->profileID == pId.toStdString())
		{
			possibleProfileList.erase(it);
			break;
		}
	}
	for (vector<HumanBlob>::iterator it = trackingHumanBlobs.begin(); it != trackingHumanBlobs.end(); it++)
	{
		if (it->profileID == pId.toStdString())
		{
			trackingHumanBlobs.erase(it);
			break;
		}
	}
}

void ThreadForNode::run()
{

	RNG rng(12345);
	VideoCapture cap(videoLink);
	
	//vector<graph::ExitPoint> exitPoints = currentNodePtr->exitPoints;
	

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
		humanBlobs.clear();
		blobsInCutoff.clear();

		string x = this->nodeId;

		vector<Point> scaledCutoffRegions;
		resizeContour(cutoffRegion, 1.0/2.200005, 1.0/2.00000, &scaledCutoffRegions);
		
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
		//vector<Point> temp;
		//for (vector<RowBlob>::iterator i = blobs.begin(); i != blobs.end(); i++)
		//{
		//	resizeContour(i->getContour(), 2.200005, 2.000000, &temp); // camnet dataset
		//	//resizeContour(i->getContour(), 1.100005, 1.200000, &temp); // senior data set
		//	i->setContour(temp);
		//	temp.clear();
		//}
		//mockFunction(&blobs, &trackingHumanBlobs, &cap);

		// detect humanblobs
		_vProcessing.humanDetection(&blobs, &frameToBeRaped, &humanBlobs, &cap, this->videoLink, svmPointer, mysqlConnection);
		

		if (!trackingHumanBlobs.empty())
		{
			//_vProcessing.dataAssociation(&humanBlobs, &trackingHumanBlobs, &missingHumanBlobs);
			
			// map detected human blobs with tracking list and update humanBlobs, trcking list and missing list
			// when map to tracking one emit for update the central Profile list
			dataAssociation(&cap);
		}
		
		
		if (!(humanBlobs.empty())) // human >>> trackinghuman
		{
			//_vProcessing.checkInProfiles(&humanBlobs, &possibleProfileList, &missingHumanBlobs, &trackingHumanBlobs);

			// 
			checkInProfiles();
		}
		
		if (!(humanBlobs.empty()))
		{
			//_vProcessing.initTrackingObject(&humanBlobs, &trackingHumanBlobs);

			for (vector<HumanBlob>::iterator i = humanBlobs.begin(); i != humanBlobs.end(); i++)
			{
				long timeapp = cap.get(CV_CAP_PROP_POS_MSEC);
				i->profileID = nodeId + to_string(profileCount);
				profileCount += 1;
				emit sendProfileToMain(QString::fromStdString(this->nodeId), &(*i), timeapp);
				trackingHumanBlobs.push_back(*i);
			}

		}
		
		if (!(trackingHumanBlobs.empty()))
		{
			_vProcessing.kalmanCorrectAndPredict(&trackingHumanBlobs);
			updateMomentVector();

			//_vProcessing.UpdateCentralProfiles(&trackingHumanBlobs);
		}


		
		// viewing // DO NOT touch - Kasun //

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


		// final resizing and drawing //
		for (vector<HumanBlob>::iterator i = trackingHumanBlobs.begin(); i != trackingHumanBlobs.end(); i++)
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

void ThreadForNode::updateProfileList(HumanBlob* profile)
{
	bool temp = false;
	for (vector<HumanBlob>::iterator it = possibleProfileList.begin(); it != possibleProfileList.end(); it++)
	{
		if (it->profileID == profile->profileID)
			temp = true;
	}
	if (!temp)
		possibleProfileList.push_back(HumanBlob(*profile));
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

void ThreadForNode::updateMomentVector()
{
	//int framewidth = frame.cols;
	//int frameheight = frame.rows;
	for (vector<HumanBlob>::iterator i = trackingHumanBlobs.begin(); i != trackingHumanBlobs.end(); i++)
	{
		vector<Point> cpl = i->centerPointList;
		if (cpl.size() == 1)
		{
			/*if ((framewidth / 2) > cpl[cpl.size() - 1].x)
				i->dx = -100;
			else
				i->dx = 100;
			if ((frameheight / 2) > cpl[cpl.size() - 1].y)
				i->dy = -100;
			else
				i->dy = 100;*/
			i->dx = 10;
			i->dy = 10;
		}
		else
		{
			double xcml = 0;
			double ycml = 0;
			for (int i = cpl.size() - 1; i >= 1; i--)
			{
				if (i == cpl.size() - 5)
					break;
				xcml += (cpl[i].x - cpl[i - 1].x);
				ycml += (cpl[i].y - cpl[i - 1].y);
			}
			i->dx = xcml / 4;
			i->dy = ycml / 4;
		}
		i->predictedPoint = Point(cpl[cpl.size() - 1].x + i->dx, cpl[cpl.size() - 1].y + i->dy);
	}
}

void ThreadForNode::informAdjecentNodes()
{

}

void ThreadForNode::checkInProfiles()
{
	vector<size_t> tobedelHuman;
	vector<size_t> tobeDel;
	for (vector<HumanBlob>::iterator h = humanBlobs.begin(); h != humanBlobs.end(); h++)
	{
		for (vector<HumanBlob>::iterator p = possibleProfileList.begin(); p != possibleProfileList.end(); p++)
		{
			// by using FEATURE VECTORS
			if ((*h) == (*p))
			{
				trackingHumanBlobs.push_back(HumanBlob(*p));

				emit requestToFlushFromOthers(QString::fromStdString(nodeId), QString::fromStdString(p->profileID));

				//possibleProfileList.erase(p);
				//humanBlobs.erase(h);
				tobeDel.push_back(p-possibleProfileList.begin());
				tobedelHuman.push_back(h-humanBlobs.begin());
				// send message to main profile set to flush from others
			}
		}
	}
	for (int i = 0; i < tobeDel.size(); i++)
	{
		auto hu = humanBlobs.begin() + tobedelHuman[i];
		auto po = possibleProfileList.begin() + tobeDel[i];
		humanBlobs.erase(hu);
		possibleProfileList.erase(po);
	}
	tobeDel.clear();
	tobedelHuman.clear();
	for (vector<HumanBlob>::iterator h = humanBlobs.begin(); h != humanBlobs.end(); h++)
	{
		for (vector<MissingHumanBlob>::iterator m = missingHumanBlobs.begin(); m != missingHumanBlobs.end(); m++)
		{
			// by using FEATURE VECTORS
			if ((*h) == m->humanBlob)
			{
				trackingHumanBlobs.push_back(MissingHumanBlob(*m).humanBlob);

				emit requestToFlushFromOthers(QString::fromStdString(nodeId), QString::fromStdString(m->humanBlob.profileID));

				//missingHumanBlobs.erase(m);
				//humanBlobs.erase(h);
				tobeDel.push_back(m - missingHumanBlobs.begin());
				tobedelHuman.push_back(h - humanBlobs.begin());
			}
		}
	}
	for (int i = 0; i < tobeDel.size(); i++)
	{
		auto hu = humanBlobs.begin() + tobedelHuman[i];
		auto mi = missingHumanBlobs.begin() + tobeDel[i];
		humanBlobs.erase(hu);
		missingHumanBlobs.erase(mi);
	}
	tobeDel.clear();
	tobedelHuman.clear();

	// update missing list
	for (vector<MissingHumanBlob>::iterator m = missingHumanBlobs.begin(); m != missingHumanBlobs.end(); m++)
	{
		m->missedTime++;
		if (m->missedTime > 10)
		{
			// give warning to main that one missed for long time
			qDebug() << "profile : " << QString::fromStdString(m->humanBlob.profileID) << " MISSING from node [" << QString::fromStdString(nodeId);
			//missingHumanBlobs.erase(m);
			tobeDel.push_back(m - missingHumanBlobs.begin());
		}
	}
	for (int i = 0; i < tobeDel.size(); i++)
	{
		auto mi = missingHumanBlobs.begin() + tobeDel[i];
		missingHumanBlobs.erase(mi);
	}
	tobeDel.clear();
}

void ThreadForNode::dataAssociation(VideoCapture* cap)
{

	/*for (vector<RowBlob>::iterator i = blobs->begin(); i != blobs->end(); i++)
	{
	outUnidentifiedBlobs->push_back(*i);
	}*/

	vector<size_t> tobedeleted;
	for (vector<HumanBlob>::iterator t = trackingHumanBlobs.begin(); t != trackingHumanBlobs.end(); t++)
	{
		double range = sqrt(pow(t->dx, 2) + pow(t->dy, 2));
		if (range < 50.0)
			range = 50.0;
		bool foundMatch = false;
		size_t n;
		double minDis = 2 * range;
		for (vector<HumanBlob>::iterator b = humanBlobs.begin(); b != humanBlobs.end(); b++)
		{
			Point detected = b->blob.getmassCenter();
			double xdis = std::abs(t->predictedPoint.x - detected.x);
			double ydis = std::abs(t->predictedPoint.y - detected.y);
			double distPredDet = sqrt(pow(xdis, 2) + pow(ydis, 2));
			if (distPredDet <= range && distPredDet < minDis)
			{
				minDis = distPredDet;
				n = b - humanBlobs.begin();
				foundMatch = true;
			}
		}
		if (foundMatch)
		{
			auto it = humanBlobs.begin() + n;
			t->blob.setContour(it->blob.getContour());
			t->addCenterPoint(t->blob.getmassCenter());
			emit sendLogCentralProfiles(QString::fromStdString(t->profileID), QString::fromStdString(nodeId), cap->get(CV_CAP_PROP_POS_MSEC));
			humanBlobs.erase(it);
		}
		else
		{
			MissingHumanBlob mhb = MissingHumanBlob(*t);
			mhb.missedTime += 1;
			missingHumanBlobs.push_back(mhb);
			graph::ExitPoint ePointHit;
			if (isInExitPoint(t->centerPointList[t->centerPointList.size() - 1], &ePointHit) 
				|| isInExitPoint(t->predictedPoint, &ePointHit))
			{
				HumanBlob* sendP = new HumanBlob(*t);
				emit sendProfileToNode(sendP, QString::fromStdString(ePointHit.nodeId));
			}
			//trackingHumanBlobs.erase(t);
			tobedeleted.push_back(t - trackingHumanBlobs.begin());
		}
	}

	for (int i = 0; i < tobedeleted.size(); i++)
	{
		trackingHumanBlobs.erase(trackingHumanBlobs.begin() + tobedeleted[i]);
	}
}

bool ThreadForNode::isInExitPoint(Point cp, graph::ExitPoint* ep)
{
	// dehan
	return false;
}

void ThreadForNode::mockFunction(vector<RowBlob> *blobs, vector<HumanBlob> *trackingHumanBlobs, VideoCapture *cap)
{
	trackingHumanBlobs->clear();
	vector<vector<Point>> blobContourVector;
	for (vector<RowBlob>::iterator it = blobs->begin(); it != blobs->end(); it++)
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
	vector<BlobId> profiledBlobs = blbDetection.matchProfilesWithBlobs(blobContourVector, timeStr, tokens[tokens.size()-1],mysqlConnection);
	for (int i = 0; i < profiledBlobs.size(); i++)
	{
		HumanBlob hb = HumanBlob(blobs->at(i));
		hb.profileID = profiledBlobs[i].Id;
		trackingHumanBlobs->push_back(hb);
	}
}