#ifndef PROFILELOG_H
#define PROFILELOG_H

#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>
#include "QtCore\qobject.h"

using namespace std;
using namespace cv;

class ProfileLog
{
	//Q_OBJECT

public:
	ProfileLog();
	~ProfileLog();

	string cameraNodeId;
	time_t timeAppeared;
private:
	
};

#endif // PROFILELOG_H
