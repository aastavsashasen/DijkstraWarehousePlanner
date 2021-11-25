// clearpathChallenge.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
using namespace std;

void printvec(vector <int> const &a) {
	cout << "The vector elements are : " << endl;

	for (int i = 0; i < a.size(); i++)
		cout << a.at(i) << ' ';
}

struct node
{
	bool visited = false;
	float globalGoal; // goals as per A* alg
	float localGoal = std::numeric_limits<float>::infinity(); // set to infinity initially (we havent looked yet)
	int nodeID;
	int x;
	int y;
	int penalty;
	int wait;
	vector<node*> vecNeighbours; // points to other node structures (neighbours including skips)
	node* parent; // points to self
};

// Can either input a single line seperated by spaces
// OR you can enter the data line by line

int main()
{
	node *nodes = nullptr; // this holds an array of nodes
	static int gridSize = 100; // optional to change for different sized 'maps'
	// get number of waypoints
	string waypointNodesstr;
	getline(cin, waypointNodesstr);
	istringstream issInit(waypointNodesstr);
	vector<int> inputValsInit; // store string stream vals
	int inputNumInit;
	while (issInit >> inputNumInit) { // streams number till white space is encountered
		inputValsInit.push_back(inputNumInit);
	}
	int waypointNodes = inputValsInit[0]; // number of waypoint nodes in first value
	const int nodeNumber = inputValsInit[0] + 2;


	// check if entered values are all in 1 line
	if (inputValsInit.size() > 1) 
	{ //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> All input data is in a single line
		// check that the right number of elements inputted
		if (((inputValsInit.size() - 1) % 3) != 0)
		{
			// also add needed number of groups of 3 (IGNORED)
			cout << "Invalid input! Node data not in groups of 3" << endl;
			return 4;
		}
		nodes = new node[nodeNumber]; // we include the start and end nodes
		// Write the data to the nodes
		for (int i = 1; i <= waypointNodes; i++)
		{
			cout << "writing to node: " << i << endl;
			int writeIdx = i * 3 - 2; // this counts 1,2,3 -> 4,5,6 -> ...
			cout << "write start index: " << writeIdx << endl;
			nodes[i].nodeID = i;
			nodes[i].x = inputValsInit[writeIdx];
			nodes[i].y = inputValsInit[writeIdx + 1];
			nodes[i].penalty = inputValsInit[writeIdx + 2];
			nodes[i].parent = nullptr;
			nodes[i].wait = 10; // all the checkponts have wait times
		}
	}
	else
	{ //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> values are being entered line by line
		//cin >> waypointNodes;
		//cin.ignore();
		//cout << "you entered: " << waypointNodes << endl;
		//const int nodeNumber = waypointNodes + 2;
		// create an array of pointers to the nodes with N nodes
		nodes = new node[nodeNumber]; // we include the start and end nodes
		//cout << "nodes array created, N: " << nodeNumber << endl;
		string input; // gets input string
		// append inputted values to an array
		vector<int> inputVals;
		for (int i = 1; i <= waypointNodes; i++)
		{
			getline(cin, input);
			istringstream iss(input);
			// clear inputVals
			inputVals.clear();
			int inputNum; // capture each number from input line
			while (iss >> inputNum) { // streams number till white space is encountered
				inputVals.push_back(inputNum);
				//cout << "input number is: " << inputNum << endl;
			}
			// check that the right number of elements inputted
			if ((inputVals.size() % 3) != 0)
			{
				cout << "Invalid input! Node data not in groups of 3" << endl;
				return 4;
			}
			// write data to node
			//cout << "writing to node: " << i << endl;
			//cout << "x: " << inputVals[0] << endl;
			//cout << "y: " << inputVals[1] << endl;
			//cout << "penalty: " << inputVals[2] << endl;
			nodes[i].nodeID = i;
			nodes[i].x = inputVals[0];
			nodes[i].y = inputVals[1];
			nodes[i].penalty = inputVals[2];
			nodes[i].parent = nullptr;
			nodes[i].wait = 10; // all the checkponts have wait times
		}
	}


	//============================================================================
	// now add the start and end nodes
	//cout << "adding start and end nodes" << endl;
	nodes[0].nodeID = 0; // start
	nodes[0].x = 0;
	nodes[0].y = 0;
	nodes[0].penalty = 0;
	nodes[0].parent = nullptr;
	nodes[0].wait = 0;
	nodes[0].localGoal = 0;

	nodes[waypointNodes + 1].nodeID = waypointNodes + 1; // end
	nodes[waypointNodes + 1].x = gridSize;
	nodes[waypointNodes + 1].y = gridSize;
	nodes[waypointNodes + 1].penalty = 0;
	nodes[waypointNodes + 1].parent = nullptr;
	nodes[waypointNodes + 1].wait = 10;
	nodes[waypointNodes + 1].localGoal = std::numeric_limits<float>::infinity();

	// Create structure to stores time costs of each possible node to node travel
	// This takes the shape of a cost matrix
	//cout << "making cost matrix of time costs" << endl;
	// a dynamic matrix
	float** costMatrix = new float*[nodeNumber];
	for (int i = 0; i < nodeNumber; i++)
	{
		costMatrix[i] = new float[nodeNumber];
	}
	//cout << "costMatrix[0][0]: " << costMatrix[0][0] << endl;
	//cout << "costMatrix[0][2]: " << costMatrix[0][2] << endl;
	//cout << "costMatrix[2][2]: " << costMatrix[2][4] << endl;
	// zero the whole matrix
	for (int j = 0; j < nodeNumber; j++)
	{
		for (int k = 0; k < nodeNumber; k++)
		{
			costMatrix[j][k] = 0;
		}
	}
	//cout << "after zeroing... " << endl;
	//cout << "costMatrix[0][0]: " << costMatrix[0][0] << endl;
	//cout << "costMatrix[0][2]: " << costMatrix[0][2] << endl;
	//cout << "costMatrix[2][2]: " << costMatrix[2][4] << endl;
	// now populate it (with time costs)
	for (int j = 0; j < nodeNumber; j++)
	{
		for (int k = j + 1; k < nodeNumber; k++)
		{
			//cout << "Calc cost from node: " << j << endl;
			//cout << "........... to node: " << k << endl;
			float travTime = sqrt(pow((nodes[j].x - nodes[k].x), 2) + pow((nodes[j].y - nodes[k].y), 2)) / 2;
			//cout << "travTime: " << travTime << endl;
			float waitTime = nodes[k].wait;
			//cout << "waitTime: " << waitTime << endl;
			// penalty time is more complicated...
			float penaltyTime = 0;
			int nodesInBtw = k;
			while (nodesInBtw > j + 1)
			{
				//cout << "nodesInBtw: " << nodesInBtw << endl;
				//cout << "j+1: " << j + 1 << endl;
				penaltyTime = penaltyTime + nodes[nodesInBtw - 1].penalty;
				//cout << "node penaltyTime: " << nodes[nodesInBtw - 1].penalty << endl;
				nodesInBtw = nodesInBtw - 1;
				//cout << "next nodesInBtw: " << nodesInBtw << endl;
			}
			//cout << "final penaltyTime: " << penaltyTime << endl;
			costMatrix[j][k] = travTime + waitTime + penaltyTime;
			//cout << "total time cost: " << travTime + waitTime + penaltyTime << endl;
		}
	}
	// display that cost matrix (upper triangular)
//	for (int j = 0; j < nodeNumber; j++)
//	{
//		for (int k = 0; k < nodeNumber; k++)
//		{
//			cout << costMatrix[j][k] << "\t";
//		}
//		cout << endl;
//	}
	// Now apply path finding (Dijkstra's)
	// If the cost of a transition is zero, do not search there (cost will never be zero) ie) sparse matrix storage
	//cout << "Now applying Dijkstra's " << endl;
	for (int i = 0; i < nodeNumber; i++)
	{
		for (int j = 0; j < nodeNumber; j++)
		{
			if (j > i)  // or could do if (j>i) then process (upper triangular)
			{
				// update local cost in node, only if its the shortest path to that node
				//cout << "from node: " << i << endl;
				//cout << "..to node: " << j << endl;
				//cout << "current time cost: " << nodes[j].localGoal << endl;
				//cout << "new path cost: " << costMatrix[i][j] + nodes[i].localGoal << endl;
				if (costMatrix[i][j] + nodes[i].localGoal < nodes[j].localGoal)
				{
					nodes[j].localGoal = costMatrix[i][j] + nodes[i].localGoal;
				}
				//cout << "new time cost: " << nodes[j].localGoal << endl;
			}
		}
	}

	//cout << "======================================= " << endl;
	float rounded = round(nodes[nodeNumber - 1].localGoal * 1000.0) / 1000.0;
	//cout << "Final time cost: " << rounded << endl;
	cout << rounded << endl;
	cout << "Press ENTER to exit..." << endl;
	cin.get();
	return 0;
	//std::cout << "Hello World!\n"; 
}
