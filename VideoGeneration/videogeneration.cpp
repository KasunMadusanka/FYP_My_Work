#include "videogeneration.h"
#include "QtWidgets\qmessagebox.h"



VideoGeneration::VideoGeneration(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect("tcp://127.0.0.1:3306", "root", "root");
	stmt = con->createStatement();
}

VideoGeneration::~VideoGeneration()
{

}

vector<string> stringSplit(string s,string delimiter=" " ){

	vector<string> splittedStrings;
	size_t pos = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		//std::cout << token << std::endl;
		splittedStrings.push_back(token);
		s.erase(0, pos + delimiter.length());

	}
	splittedStrings.push_back(s);
	return splittedStrings;
}
void VideoGeneration::on_generateVideoPushButton_clicked()
{
	//string camPath = "C:\\Users\\dehandecroos\\Desktop\\Videos\\PRG28.avi";
	QString profileId = ui.profileName_lineEdit->text();


	
	string cameraIds[] = { 
		"cam6_video", 
		"cam1_video", 
		"cam28_video",
		"cam23_video"
	};
	int cameraIdsSize = sizeof(cameraIds) / sizeof(*cameraIds);
	string finalJoinQuery = "";
	int i = 1;
	for (string cameraId : cameraIds)
	{
		finalJoinQuery += "select * from " + cameraId + " where profile_id='" + profileId.toStdString() + "'";
		if (i++ != cameraIdsSize) {
			finalJoinQuery += "union ";
		}
	}
	finalJoinQuery += "order by TimeStamp";
	
	struct CameraTimeStamp{
		string cameraId;
		double timestamp;
	};
	
	
	stmt->execute("USE camera");
	ResultSet *timeStampsForProfile = stmt->executeQuery(finalJoinQuery);
	vector<CameraTimeStamp> timeStamps;
	
	while (timeStampsForProfile->next())
	{
		CameraTimeStamp timeStamp;
		timeStamp.cameraId = timeStampsForProfile->getString("Video_ID");
		timeStamp.timestamp = timeStampsForProfile->getDouble("TimeStamp");
		timeStamps.push_back(timeStamp);
	}

	
	vector<Mat> video;
	for (CameraTimeStamp ts : timeStamps)
	{
		string camPath = "C:\\AdaptiveCameraNetworkPack\\Videos\\";
		string camId = ts.cameraId;
		camPath += "PRG" + camId + ".avi";
		VideoCapture cam;
		cam.open(camPath);
		int frameRate = cam.get(CV_CAP_PROP_FPS);

		int minutes = (int)ts.timestamp;
		int seconds = (int)((ts.timestamp-(double)minutes)*100.0);

		
		int milliseconds = (minutes * 60 + seconds )* 1000;
		cam.set(CV_CAP_PROP_POS_MSEC, milliseconds);
		
		
		for (int frameCount = 0; frameCount < frameRate; frameCount++)
		{
			Mat frame;
			cam >> frame;
			if (frame.empty())
			{
				break;
			}
			int fontFace = FONT_HERSHEY_SIMPLEX;
			double fontScale = 1;
			int thickness = 3;
			cv::Point textOrg1(10, 50);
			putText(frame, "CAM:" + ts.cameraId, textOrg1, fontFace, fontScale, Scalar::all(250),2);
			cv::Point textOrg2(500, 50);
			video.push_back(frame);
		}
		
		

		//VideoCapture

	}

	if (video.size() == 0){
		QImage finalImage(ui.lblOutput->width(), ui.lblOutput->width(), QImage::Format_RGB888);
		QPainter qp(&finalImage);
		qp.setBrush(Qt::black);
		qp.setPen(Qt::red);
		qp.setFont(QFont("Times", 12, QFont::Bold));
		qp.drawText(finalImage.rect(), Qt::AlignCenter, "NO VIDEO FOR "+ profileId);
		ui.lblOutput->setPixmap(QPixmap::fromImage(finalImage));
	}
	else
	{
		for (Mat frameZ : video)
		{
			Mat frameForQimage;
			cvtColor(frameZ, frameForQimage, CV_BGR2RGB);
			QImage outImage((uchar*)frameForQimage.data, frameForQimage.cols, frameForQimage.rows, frameZ.step, QImage::Format_RGB888);
			ui.lblOutput->setPixmap(QPixmap::fromImage(outImage));
			imshow("Output", frameZ);
			cvWaitKey(1);

		}
	}
}
