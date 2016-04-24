#include <algorithm>
#include "Block.h"

Block::Block():mID(0), mNodeList(NULL), InSDist(NULL), HaveAdjMatrix(false), HaveNodeToPortal(false), mPNodePos(0) {};

Block::Block(int ID, int NodeNum, int *NodeList):mID(ID), HaveAdjMatrix(false), HaveNodeToPortal(false), mPNodePos(NodeNum) {
	mNodeList = new std::vector<int>(NodeList, NodeList + NodeNum); //nodenum > 0
}

Block::Block(const Block& cpy) {
	mID = cpy.mID;

	HaveAdjMatrix = cpy.HaveAdjMatrix;

	HaveNodeToPortal = cpy.HaveNodeToPortal;

	mPNodePos = cpy.mPNodePos;

	//construct Nodelist array
	mNodeList = new std::vector<int>(*(cpy.mNodeList));
} 

Block::~Block() {

	if (HaveAdjMatrix) {

		int mNodeNum = mNodeList->size();

		for (int i = 0; i < mNodeNum; ++i) {

			delete [] InSDist[i];
		}

		delete [] InSDist;

	}

	if (HaveNodeToPortal) {

		int mNodeNum = mNodeList->size();

		for (int i = 0; i < mNodeNum; ++i) {

			delete [] NodeToPortal[i];
		}

		delete [] NodeToPortal;
	}

	delete mNodeList;
}

void Block::Floyd() {

	int mNodeNum = mNodeList->size();

	for (int k = 0; k < mNodeNum; ++k) {
		for(int i = 0; i < mNodeNum; ++i) {
			if (!InSDist[i][k]) continue; //i to k unreachable
			for (int j = 0; j < mNodeNum; ++j) {
				if (i == j) continue; // i to i
				if (!InSDist[k][j]) continue; //k to j unreachable

				int temp = InSDist[i][k] + InSDist[k][j];

				if (!InSDist[i][j] || temp < InSDist[i][j]) {
					InSDist[i][j] = temp;
				}
			}
		}
	}
}

void Block::NodeListToAdjMatrix() {

	int mNodeNum = mNodeList->size();

	for (int node = 0; node < mNodeNum; ++node) {
		for (int i = 0; i < mNodeNum; ++i) {
			InSDist[node][i] = AdjMatrix[(*mNodeList)[node]][(*mNodeList)[i]];
		}
	}
}

void Block::ConstructAdjMatrix() {

	int mNodeNum = mNodeList->size();

	InSDist = new int *[mNodeNum];

	for (int i = 0; i < mNodeNum; ++i) {
		InSDist[i] = new int[mNodeNum];
	}

	HaveAdjMatrix = true;
}

void Block::CalNodeToPortalIndex() {

	int mNodeNum = mNodeList->size();

	NodeToPortal = new NodeDist *[mNodeNum];

	int PortalCount = mNodeNum - mPNodePos;

	for (int node = 0; node < mNodeNum; ++node) {
		NodeToPortal[node] = new NodeDist[PortalCount];
	}

	HaveNodeToPortal = true;

	//for each node in block except portal node
	for (int node = 0; node < mNodeNum; ++node) {
		
		//get the node to all portal nodes shortest distance and sort
		for (int portal = mPNodePos, count = 0, end = mNodeList->size(); portal < end; ++portal, ++count) {

			NodeToPortal[node][count].NodeID = portal;
			NodeToPortal[node][count].Dist = InSDist[node][portal];

		}

		std::sort(NodeToPortal[node], NodeToPortal[node] + PortalCount);
	}
}