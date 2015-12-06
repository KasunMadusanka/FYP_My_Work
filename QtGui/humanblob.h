#ifndef HUMANBLOB_H
#define HUMANBLOB_H

#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>
#include "QtCore\qobject.h"
#include "rowblob.h"

using namespace cv;
using namespace std;

class HumanBlob
{
	//Q_OBJECT

public:
	HumanBlob();
	HumanBlob(RowBlob superBlob);
	bool operator==(const HumanBlob& human);
	void addCenterPoint(Point p);
	~HumanBlob();

	RowBlob blob;
	vector<Point> centerPointList;
	KalmanFilter kalmanFilter;
	string profileID;
	// movement vector
	Point predictedPoint;
	double dx;
	double dy;
private:
	
};

#endif // HUMANBLOB_H
