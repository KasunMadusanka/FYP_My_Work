#ifndef CENTRALPROFILE_H
#define CENTRALPROFILE_H

#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>
#include "QtCore\qobject.h"
#include "humanblob.h"
#include "profilelog.h"

using namespace std;
using namespace cv;

class CentralProfile
{
	//Q_OBJECT

public:
	CentralProfile();
	~CentralProfile();

	string profileId;
	HumanBlob humanObj;
	vector<ProfileLog> profileLogs;
private:
	
};

#endif // CENTRALPROFILE_H
