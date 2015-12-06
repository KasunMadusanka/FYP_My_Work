#include "qttesting.h"

QtTesting::QtTesting(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	vector<QLabel*> uiLblVector{ ui.lblCamera1, ui.lblCamera2, ui.lblCamera3, ui.lblCamera4, ui.lblCamera5, ui.lblCamera6, ui.lblCamera7, ui.lblCamera8 };

	graph::Graph _graph = graph::Graph();
	_graph.initGraph();

	svmCam.load_svm("camera6.xml");

	MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
	Connection* mysqlConnection = driver->connect("tcp://127.0.0.1:3306", "root", "root");
	ProfileHits* pLogger = new ProfileHits(mysqlConnection);
	pLogger->initDB();
	delete pLogger;
	pLogger = NULL;
	delete mysqlConnection;
	mysqlConnection = NULL;

	vector<graph::Node> nodes = _graph.getAllNodes();

	for (vector<graph::Node>::iterator it = nodes.begin(); it != nodes.end(); it++)
	{
		ThreadForNode* thread = new ThreadForNode();
		thread->nodeId = it->nodeId;
		thread->videoLink = it->videoLink;
		thread->exitPoints = it->exitPoints;
		thread->startFrame = it->startFrame;
		thread->isNotShown = &isNotShown;
		thread->mutex = &mutex;
		thread->svmPointer = &svmCam;

		MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
		thread->mysqlConnection = driver->connect("tcp://127.0.0.1:3306", "root", "root");
		if (it->nodeId == "C008")
		{
			thread->cutoffRegion.push_back(Point(0, 0));
			thread->cutoffRegion.push_back(Point(170, 0));
			thread->cutoffRegion.push_back(Point(180, 290));
			thread->cutoffRegion.push_back(Point(110, 450));
			thread->cutoffRegion.push_back(Point(0, 480));
		}

		nodeToThreadMap[it->nodeId] = thread;

	}

	for (map<string, ThreadForNode*>::iterator it = nodeToThreadMap.begin(); it != nodeToThreadMap.end(); it++)
	{
		connect(it->second, SIGNAL(sendFrameToMain(QImage, ThreadForNode*)),
			this, SLOT(recieveFrameFromThreads(QImage, ThreadForNode*)));
		connect(it->second, SIGNAL(sendFinishedToMain()),
			this, SLOT(finishedThreads()));
		connect(it->second, SIGNAL(sendProfileToMain(QString, HumanBlob*, long)),
			this, SLOT(profileCreatedInNode(QString, HumanBlob*, long)));
		connect(it->second, SIGNAL(sendLogCentralProfiles(QString, QString, long)),
			this, SLOT(logCentralProfiles(QString, QString, long)));
		connect(it->second, SIGNAL(sendProfileToNode(HumanBlob*, QString)),
			this, SLOT(passProfile(HumanBlob*, QString)));
	}

	for (int i = 0; i < nodes.size(); i++)
	{
		this->lblMap[nodes[i].nodeId] = uiLblVector[i];
	}

	for (map<string, ThreadForNode*>::iterator it = nodeToThreadMap.begin(); it != nodeToThreadMap.end(); it++)
	{
		if (it->second->startFrame == 0)
		{
			it->second->releventUiLable = lblMap[it->first];
			this->threadCount += 1;
			it->second->start();
		}
		else
		{
			toBeStart.push_back(it->first);
		}
	}
}

QtTesting::~QtTesting()
{

}


void QtTesting::recieveFrameFromThreads(QImage outImage, ThreadForNode* thread)
{
	if (completedThreadMap.count(thread->nodeId) == 0)
	{
		frameMap[thread->nodeId] = outImage;
		completedThreadMap[thread->nodeId] = thread;
	}
	if (completedThreadMap.size() == threadCount)
	{
		for (map<string, QImage>::const_iterator i = frameMap.begin(); i != frameMap.end(); i++)
		{
			lblMap[i->first]->setPixmap(QPixmap::fromImage(frameMap[i->first]));
		}
		for (map<string, ThreadForNode*>::const_iterator i = completedThreadMap.begin(); i != completedThreadMap.end(); i++)
		{
			i->second->acknowledged = true;
		}

		mutex.lock();
			isNotShown.wakeAll();
		mutex.unlock();
		
		ui.lblGlobalFrameCounter->setText(QString::number(globalFrameCount));

		globalFrameCount += 1;

		/*if (globalFrameCount == 129)
			__debugbreak();*/

		wakeForFrameCount();

		completedThreadMap.clear();
		frameMap.clear();
	}
}

void QtTesting::finishedThreads()
{
	threadCount -= 1;
}

void QtTesting::wakeForFrameCount()
{
	vector<string> temp;
	for (int i = 0; i < toBeStart.size(); i++)
	{
		if (nodeToThreadMap[toBeStart[i]]->startFrame == globalFrameCount)
		{
			nodeToThreadMap[toBeStart[i]]->releventUiLable = lblMap[toBeStart[i]];
			nodeToThreadMap[toBeStart[i]]->start();
			this->threadCount += 1;
		}
		else
		{
			temp.push_back(toBeStart[i]);
		}
	}

	toBeStart = temp;
}

void QtTesting::profileCreatedInNode(QString cameraNode, HumanBlob* humanBlob, long time)
{
	muForProfileCreation.lock();

	// match in central profiles whether already Ided.
	CentralProfile cp;
	cp.profileId = humanBlob->profileID;
	cp.humanObj = *humanBlob;
	ProfileLog pl;
	pl.cameraNodeId = cameraNode.toStdString();
	pl.timeAppeared = time;
	cp.profileLogs.push_back(pl);

	centralProfileList.push_back(cp);

	centralNodeToHumanMap[cameraNode.toStdString()].push_back(humanBlob->profileID);
	centralHumanToNodeMap[humanBlob->profileID] = cameraNode.toStdString();

	muForProfileCreation.unlock();
}

void QtTesting::logCentralProfiles(QString pId, QString nId, long time)
{
	ProfileLog pl;
	pl.cameraNodeId = nId.toStdString();
	pl.timeAppeared = time;
	for (int i = 0; i < centralProfileList.size(); i++)
	{
		if (centralProfileList[i].profileId == pId.toStdString())
		{
			centralProfileList[i].profileLogs.push_back(pl);
		}
	}
}

void QtTesting::flushFromOthers(QString nId, QString pId)
{
	for (map<string, ThreadForNode*>::iterator it = nodeToThreadMap.begin(); it != nodeToThreadMap.end(); it++)
	{
		if (it->first != nId.toStdString()) // omit original
		{
			emit it->second->flushProfile(pId);
		}
	}
}

void QtTesting::passProfile(HumanBlob* profile, QString sendingNodeId)
{
	ThreadForNode* threadToSend = nodeToThreadMap[sendingNodeId.toStdString()];
	emit threadToSend->receiveProfile(profile);
}