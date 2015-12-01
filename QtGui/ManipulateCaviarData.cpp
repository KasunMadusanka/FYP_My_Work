#include <QtCore\qcoreapplication.h>
#include <QtCore\QDirIterator>
#include <QtCore\QDir>
#include <QtCore\QDebug>
#include <QtCore\QFileInfo>
#include <QtCore\QStringList>
#include <QApplication>
#include <QLabel>
#include <windows.h>
#include <cstdio>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <ManageBlobExtracts.h>
#include "MomentCalculation2.h"
#include <humanhits.h>
#include <caviar_hits.h>


using namespace cv;
using namespace std;
struct ColorMoments{
	double ch1Av;
	double ch2Av;
	double ch3Av;

	double ch1stdDev;
	double ch2stdDev;
	double ch3stdDev;

	double ch1Sk;
	double ch2Sk;
	double ch3Sk;
};

struct HumanProportions{
	int height = 14;
	int head = 2;
	int gait = 5;
	int leg = 7;

	int currentHeight;
	int currentHead;
	int currentGait;
	int currentLeg;

};


void main(int argc, char *argv[])
{
	//QCoreApplication app(argc, argv);
	QApplication app(argc, argv);
	caviar_hits cc;


	/*string concat = "";
	for (int i = 10; i < 73; i++)
	{

		string x = "update caviar_hits_comparison set testing_image_human_id = " + to_string(i) + " where testing_image like '00"+to_string(i)+"%';";
		concat += x;
	}*/
	//select count(TruePositive), count(TrueNegative), count(FalseNegative), count(FalsePositive) from caviar_hits_comparison;
	// UPDATE caviar_hits_comparison set TruePositive=TRUE where verdict=true and testing_image_human_id = control_image_human_id;
	//UPDATE caviar_hits_comparison set Verdict=true where distance < 20;
	//select * from caviar_hits_comparison where control_image like '0070%';
	
	cc.compareAllHits();

	
		//std::auto_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("SELECT id, label FROM test ORDER BY id ASC"));
		//std::auto_ptr< sql::ResultSet > res(prep_stmt->executeQuery());




	
}