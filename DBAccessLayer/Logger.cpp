#include "Logger.h"
#include <windows.h> // for Sleep


ProfileHits::ProfileHits(Connection *con)
{
	this->con = con;
	this->stmt = this->con->createStatement();

	
}
ProfileHits::ProfileHits()
{

	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect("tcp://127.0.0.1:3306", "root", "root");


}




ProfileHits::~ProfileHits()
{
	this->driver = NULL;
	this->con = NULL;
}


void ProfileHits::profileLog(string Video_ID, string Profile_ID, string TimeStamp)
{
	
	stmt->execute("USE camera");
	string Video_DB_ID;
	map<string, string> dbLinkMap;
	map<string, int> dbIdMap;
	dbLinkMap["C:/AdaptiveCameraNetworkPack/Videos/PRG1.avi"]="camera_node_1_log";
	dbIdMap["C:/AdaptiveCameraNetworkPack/Videos/PRG1.avi"]=1;
	dbLinkMap["C:/AdaptiveCameraNetworkPack/Videos/PRG6.avi"]="camera_node_6_log";
	dbIdMap["C:/AdaptiveCameraNetworkPack/Videos/PRG6.avi"]=6;
	dbLinkMap["C:/AdaptiveCameraNetworkPack/Videos/PRG7.avi"]="camera_node_7_log";
	dbIdMap["C:/AdaptiveCameraNetworkPack/Videos/PRG7.avi"]=7;
	dbLinkMap["C:/AdaptiveCameraNetworkPack/Videos/PRG14.avi"]="camera_node_14_log";
	dbIdMap["C:/AdaptiveCameraNetworkPack/Videos/PRG14.avi"]=14;
	dbLinkMap["C:/AdaptiveCameraNetworkPack/Videos/PRG22.avi"]="camera_node_22_log";
	dbIdMap["C:/AdaptiveCameraNetworkPack/Videos/PRG22.avi"]=22;
	dbLinkMap["C:/AdaptiveCameraNetworkPack/Videos/PRG23.avi"]="camera_node_23_log";
	dbIdMap["C:/AdaptiveCameraNetworkPack/Videos/PRG23.avi"]=23;
	dbLinkMap["C:/AdaptiveCameraNetworkPack/Videos/PRG28.avi"]="camera_node_28_log";
	dbIdMap["C:/AdaptiveCameraNetworkPack/Videos/PRG28.avi"]=28;
	dbLinkMap["C:/AdaptiveCameraNetworkPack/Videos/PRG29.avi"]="camera_node_29_log";
	dbIdMap["C:/AdaptiveCameraNetworkPack/Videos/PRG29.avi"]=29;


	string queryInsert = "INSERT INTO " + dbLinkMap[Video_ID] + "(Video_ID, Profile_ID, TimeStamp) VALUES("
		+ to_string(dbIdMap[Video_ID]) + ",'"
		+ Profile_ID + "'," 
		+ TimeStamp + ")";

	stmt->executeUpdate(queryInsert);



}

void ProfileHits::initDB()
{
	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect("tcp://127.0.0.1:3306", "root", "root");

	stmt = con->createStatement();
	stmt->execute("USE camera");

	string Video_IDs[] = { 
		"camera_node_1",
		"camera_node_6_log",
		"camera_node_7_log",
		"camera_node_14_log",
		"camera_node_22_log",
		"camera_node_23_log",
		"camera_node_28_log",
		"camera_node_29_log" };

	for (string id : Video_IDs)
	{
		string deletQuery = "DELETE FROM " + id;
		stmt->execute(deletQuery);
	}
	

	


	delete con;
	delete stmt;


}
