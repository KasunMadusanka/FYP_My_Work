#ifndef ROWBLOB_H
#define ROWBLOB_H

#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>
#include "QtCore\qobject.h"

using namespace std;
using namespace cv;

class RowBlob
{
	//Q_OBJECT

public:
	RowBlob();
	RowBlob(vector<Point> _contour);
	~RowBlob();

	vector<Point> getContour();
	Moments getMoments();
	Point2f getmassCenter();
	void setContour(vector<Point> _contour);
private:
	vector<Point> contour;
	Moments mu;
	Point2f mc;
};

#endif // ROWBLOB_H
