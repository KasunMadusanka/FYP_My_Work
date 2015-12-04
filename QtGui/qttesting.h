#ifndef QTTESTING_H
#define QTTESTING_H

#include <QtWidgets/QMainWindow>
#include "ui_qttesting.h"

#include <iostream>
#include "threadfornode.h"
#include "svm__class.h"
#include "Logger.h"

class QtTesting : public QMainWindow
{
	Q_OBJECT

public:
	QtTesting(QWidget *parent = 0);
	~QtTesting();

	QWaitCondition isNotShown;
	QMutex mutex;
	SVM__Class svmCam;

	public slots:
	void recieveFrameFromThreads(QImage outImage, ThreadForNode* thread);
	void finishedThreads();
	void wakeForFrameCount();
	

private:
	Ui::QtTestingClass ui;

	map<string, ThreadForNode*> nodeToThreadMap;

	map<string, ThreadForNode*> completedThreadMap;
	map<string, QImage> frameMap;
	map<string, QLabel*> lblMap;
	int threadCount = 0;
	int globalFrameCount = 1;
	vector<string> toBeStart;
};

#endif // QTTESTING_H
