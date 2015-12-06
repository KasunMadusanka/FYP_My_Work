#ifndef SVM__CLASS_H
#define SVM__CLASS_H

#include <QtCore\qstring.h>
#include <QtCore\qdiriterator.h>
#include <QtCore\qstring.h>
#include <QtCore\qstring.h>
#include <QtCore\qdebug.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>


#include "hogdescriptor_class.h"

using namespace cv; 
using namespace std;

class SVM__Class
{
public:
	SVM__Class();
	~SVM__Class();

	int positive_training_count = 0;
	int negative_training_count = 0;
	int total_training_count = 0;
	int total_testing_count = 0;
	int predicted_postive_count = 0;
	int predicted_negative_count = 0;
	Mat training_Descriptor;
	Mat testing_Descriptor; 
	Mat training_lable;
	Mat testing_result;
	Mat negative_f;
	Mat positive_f;
	Mat predicted_results;
	CvSVM svm;
	CvSVMParams params;
	
	

	void SVM__Class::positiveTrainingSet(QString path);
	void SVM__Class::positiveTrainingSet(QString path, int number);				//overloaded mtd to take limited number of files
	void SVM__Class::negativeTrainingSet(QString path);	
	void SVM__Class::negativeTrainingSet(QString path, int number);				//overloaded mtd to take limited number of files
	void SVM__Class::testingSet(Mat imagte_toPredict);
	void SVM__Class::testingSet(QString path);		//overloaded mtd to take limited number of files
	void SVM__Class::train_svm();
	void SVM__Class::train_svmAndSave(string filename);
	void SVM__Class::load_svm(string filename);
	void SVM__Class::predict_svm();
	bool SVM__Class::predict_Singlesvm(string filepath);
	bool SVM__Class::predict_SingleSVMfromMat(Mat image);
	void SVM__Class::show_predictionReport();
	void SVM__Class::fileIterator(QString filepath, Mat *descriptor, int *counter,char lable);
	void SVM__Class::print_Mat(Mat *mat_toprint);
	

private:
	vector<float> svmPredictedValues;
	/*int positive_training_count = 0;
	int negative_training_count = 0;
	int total_training_count = 0;
	int total_testing_count = 0;*/
};

#endif // SVM__CLASS_H
