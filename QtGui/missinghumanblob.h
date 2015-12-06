#ifndef MISSINGHUMANBLOB_H
#define MISSINGHUMANBLOB_H

#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>
#include "QtCore\qobject.h"
#include "humanblob.h"

using namespace std;
using namespace cv;

class MissingHumanBlob
{
	//Q_OBJECT

public:
	MissingHumanBlob();
	MissingHumanBlob(HumanBlob superHumanBlob);
	~MissingHumanBlob();

	HumanBlob humanBlob;
	int missedTime;
private:
	
};

#endif // MISSINGHUMANBLOB_H
