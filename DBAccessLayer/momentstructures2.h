#ifndef MOMENTSTRUCTURES2_H
#define MOMENTSTRUCTURES2_H

#include "QtCore\qobject.h"
#include "dbaccesslayer_global.h"

class DBACCESSLAYER_EXPORT MomentStructures2 
{

public:
	MomentStructures2();
	double channel0;
	double channel1;
	double channel2;

private:
	
};

class DBACCESSLAYER_EXPORT MomentAverage : public MomentStructures2
{
public:
	
	
	MomentAverage(double av);
	MomentAverage(double av0, double av1, double av2);
	~MomentAverage();



};

class DBACCESSLAYER_EXPORT MomentStandardDeviation : public MomentStructures2
{

public:
	MomentStandardDeviation(double stdDev);
	MomentStandardDeviation(double stdDev0, double stdDev1, double stdDev2);
	~MomentStandardDeviation();


};

class DBACCESSLAYER_EXPORT MomentSkewness : public MomentStructures2
{

public:
	MomentSkewness(double skew0);
	MomentSkewness(double skew0, double skew1, double skew2);
	~MomentSkewness();


};

class DBACCESSLAYER_EXPORT Region
{

public:
	std::string regionId;
	int			startRow;
	int			startCol;
	int			endRow;
	int			endCol;
	Region(std::string id, int startRow, int startCol, int endRow, int endCol);
	Region();

	void setMoments(MomentAverage *momentAverage, MomentStandardDeviation *momentStandardDeviation, MomentSkewness *momentSkewness);
	void setRegionId(std::string regionId);
	MomentAverage* getAverageMoment();
	MomentStandardDeviation* getStandardDeviationMoment();
	MomentSkewness* getSkewnessMoment();
	~Region();


private:
	MomentAverage			*momentAverage;
	MomentStandardDeviation *momentStandardDeviation;
	MomentSkewness			*momentSkewness;

};





class DBACCESSLAYER_EXPORT Blob
{
public:
	std::string hitId;
	std::string path;
	std::string human_id_actual;
	int rows;
	int cols;
	std::string timeStamp;

	Blob();
	Blob(std::string id, int rows, int cols, std::string timeStamp = "");
	void addRegion(Region *region);
	void removeRegion(std::string id);
	std::vector<Region> getAllRegions();
	~Blob();


private:
	std::vector<Region> regions;

};







#endif // MOMENTSTRUCTURES2_H
