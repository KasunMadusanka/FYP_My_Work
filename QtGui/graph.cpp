#include "graph.h"
using namespace graph;
using namespace std;

Graph::Graph()
{

}

Graph::~Graph()
{

}



bool Graph::isConfigured()
{
	return true;
}

int Graph::initGraph()
{
	//graph::ExitPoint p1; p1.nodeId = "C002";
	//graph::ExitPoint p2; p2.nodeId = "C004";
	//graph::ExitPoint p3; p3.nodeId = "C005";
	//vector<graph::ExitPoint> xp1{p1, p2, p3};
	//addNode(Node("C001", "C:/AdaptiveCameraNetworkPack/Videos/PRG1.avi", xp1, 5));
	
	graph::ExitPoint p4; p4.nodeId = "C001";
	graph::ExitPoint p5; p5.nodeId = "C003";
	graph::ExitPoint p6; p6.nodeId = "C004";
	vector<graph::ExitPoint> xp2{ p4, p5, p6 };
	addNode(Node("C002", "C:/AdaptiveCameraNetworkPack/Videos/PRG6.avi", xp2, 0));
	
	//graph::ExitPoint p7; p7.nodeId = "C002";
	//graph::ExitPoint p8; p8.nodeId = "C004";
	//graph::ExitPoint p9; p9.nodeId = "C006";
	//vector<graph::ExitPoint> xp3{ p7, p8, p9 };
	//addNode(Node("C003", "C:/AdaptiveCameraNetworkPack/Videos/PRG7.avi", xp3, 85));
	//
	//graph::ExitPoint p10; p10.nodeId = "C001";
	//graph::ExitPoint p11; p11.nodeId = "C002";
	//graph::ExitPoint p12; p12.nodeId = "C003";
	//graph::ExitPoint p13; p13.nodeId = "C005";
	//graph::ExitPoint p14; p14.nodeId = "C006";
	//graph::ExitPoint p15; p15.nodeId = "C007";
	//graph::ExitPoint p16; p16.nodeId = "C008";
	//vector<graph::ExitPoint> xp8{ p10, p11, p12, p13, p14, p15, p16 };
	//addNode(Node("C004", "C:/AdaptiveCameraNetworkPack/Videos/PRG14.avi", xp8, 0));
	//
	//graph::ExitPoint p17; p17.nodeId = "C001";
	//graph::ExitPoint p18; p18.nodeId = "C004";
	//graph::ExitPoint p19; p19.nodeId = "C007";
	//vector<graph::ExitPoint> xp4{ p10, p11, p12, p13, p14, p15, p16 };
	//addNode(Node("C005", "C:/AdaptiveCameraNetworkPack/Videos/PRG22.avi", xp4, 15));
	//
	//graph::ExitPoint p20; p20.nodeId = "C003";
	//graph::ExitPoint p21; p21.nodeId = "C008";
	//vector<graph::ExitPoint> xp5{ p20, p21 };
	//addNode(Node("C006", "C:/AdaptiveCameraNetworkPack/Videos/PRG23.avi", xp5, 0));
	//
	//graph::ExitPoint p22; p22.nodeId = "C004";
	//graph::ExitPoint p23; p23.nodeId = "C005";
	//graph::ExitPoint p24; p24.nodeId = "C008";
	//vector<graph::ExitPoint> xp6{ p22, p23,p24 };
	//addNode(Node("C007", "C:/AdaptiveCameraNetworkPack/Videos/PRG28.avi", xp6, 70));
	
	//graph::ExitPoint p25; p25.nodeId = "C004";
	//graph::ExitPoint p26; p26.nodeId = "C006";
	//graph::ExitPoint p27; p27.nodeId = "C007";
	//vector<graph::ExitPoint> xp7{ p25, p26, p27 };	
	//addNode(Node("C008", "C:/AdaptiveCameraNetworkPack/Videos/PRG29.avi", xp7, 115));

	//addNode(Node("C001", "C:/AdaptiveCameraNetworkPack/Videos/SVideo/A.mov", xp1, 0));
	//addNode(Node("C002", "C:/AdaptiveCameraNetworkPack/Videos/SVideo/B.mov", xp1, 0));
	//addNode(Node("C003", "C:/AdaptiveCameraNetworkPack/Videos/SVideo/C.mov", xp1, 0));
	//addNode(Node("C004", "C:/AdaptiveCameraNetworkPack/Videos/SVideo/D.mov", xp1, 0));
	//addNode(Node("C005", "C:/AdaptiveCameraNetworkPack/Videos/SVideo/E.mov", xp1, 0));
	//addNode(Node("C006", "C:/AdaptiveCameraNetworkPack/Videos/SVideo/F.mov", xp1, 0));
	//addNode(Node("C007", "C:/AdaptiveCameraNetworkPack/Videos/SVideo/G.mov", xp1, 0));
	//addNode(Node("C008", "C:/AdaptiveCameraNetworkPack/Videos/SVideo/H.mov", xp1, 0));

	/*addNeighbour("C001", "C002", 8, 1);
	addNeighbour("C002", "C001", 2, 1);

	addNeighbour("C001", "C003", 6, 1);
	addNeighbour("C003", "C001", 4, 1);*/

	qDebug() << "Graph initiated. \n";
	return Nodes.size();
}

vector<Node> Graph::getAllNodes()
{
	vector<Node> returnVector;
	for (map<string, Node>::const_iterator i = Nodes.begin(); i != Nodes.end(); i++)
	{
		returnVector.push_back(i->second);
	}
	return returnVector;
}

vector<string> Graph::getKeysOfAllNodes()
{
	vector<string> returnVector;
	for (map<string, Node>::const_iterator i = Nodes.begin(); i != Nodes.end(); i++)
	{
		returnVector.push_back(i->first);
	}
	return returnVector;
}

Node * Graph::getNode(string node_id)
{
	return &Nodes.at(node_id);
}

bool Graph::addNode(Node node)
{
	Nodes.insert(map<string, Node>::value_type(node.nodeId, node));
	vector<vector<pair<string, int>>> vect(9);
	Edges[node.nodeId] = vect;
	return true;
}

bool Graph::editNode(Node node)
{
	Nodes[node.nodeId].videoLink = node.videoLink;
	Nodes[node.nodeId].exitPoints = node.exitPoints;
	return true;
}

bool Graph::deleteNode(string node_id)
{
	Nodes.erase(node_id);
	return false;
}

map<string, vector<vector<pair<string, int>>>> Graph::getAdgecencyList()
{
	return Edges;
}

vector<vector<pair<string, int>>> Graph::getNeighbours(string base_node_id)
{
	return Edges.at(base_node_id);
}

bool Graph::addNeighbour(string base_node_id, Node new_node, int location_in_metrix, int weight)
{
	if (Nodes.count(new_node.nodeId) == 0)
	{
		Graph::addNode(new_node);
	}
	else
	{
		qDebug() << "# ERROR :: at addNeighbour - new node is already exist. Use other override of addNeighbour.\n";
		return false;
	}
	return addNeighbour(base_node_id, new_node.nodeId, location_in_metrix, weight);
}

bool Graph::addNeighbour(string base_node_id, string new_node_id, int location_in_metrix, int weight)
{
	if (Nodes.count(base_node_id) != 0 && Nodes.count(new_node_id) != 0)	// if both nodes are available
	{
		if (Edges.count(base_node_id) != 0)	// if base node is already in the edges collection
		{
			for (vector<pair<string, int>>::iterator i = Edges[base_node_id][location_in_metrix].begin(); i != Edges[base_node_id][location_in_metrix].end(); i++)
			{
				if ((*i).first == new_node_id)
				{
					(*i).second = weight;
					return true;
				}
			}
			Edges[base_node_id][location_in_metrix].push_back(vector<pair<string, int>>::value_type(make_pair(new_node_id, weight)));
			return true;
		}
		else
		{
			Edges[base_node_id][location_in_metrix].push_back(vector<pair<string, int>>::value_type(make_pair(new_node_id, weight)));
			return true;
		}
	}
	else
	{
		qDebug() << "# ERROR :: at addNeighbour - node ids are not exist. Add nodes before continue.\n";
		return false;
	}
}

bool Graph::removeNeighbour(string base_node_id, string removing_id)
{
	for (vector<vector<pair<string, int>>>::iterator i = Edges.at(base_node_id).begin(); i != Edges.at(base_node_id).end(); i++)
		for (vector<pair<string, int>>::iterator j = (*i).begin(); j != (*i).end(); j++)
			if ((*j).first == removing_id)
				(*i).erase(j);
	return true;
}

bool Graph::isNeighbour(string node1_id, string node2_id)
{
	for (vector<vector<pair<string, int>>>::iterator i = Edges.at(node1_id).begin(); i != Edges.at(node1_id).end(); i++)
		for (vector<pair<string, int>>::iterator j = (*i).begin(); j != (*i).end(); j++)
			if ((*j).first == node2_id)
				return true;
	return false;
}

