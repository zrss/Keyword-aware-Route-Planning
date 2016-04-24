#ifndef _BLOCK_H
#define _BLOCK_H

#include <string.h> //for memcpy
#include <vector>
#include "Graph.h" //for adjmatrix
#include "NodeDist.h" //for nodedist struct for node to portal index

#define MAX_BLOCK_NODE 400

class Block
{
public:
	Block();
	Block(int ID, int NodeNum, int *NodeList); //construct Nodelist array
	Block(const Block& cpy);
	~Block();

	void AddNode(int NodeID) { //must call after TestRepetition; avoid repetition
		mNodeList->push_back(NodeID); //add a node to block
	}

	bool TestRepetition(int NodeID) { return NodeID == mNodeList->back(); }

	int getBlockSize() {return mNodeList->size();}

	void CalIntraBlockIndex() {ConstructAdjMatrix(); NodeListToAdjMatrix(); Floyd();} //

	void CalNodeToPortalIndex();

	int mID;
	int mPNodePos; //the number of node excepte portal nodes
	
	std::vector<int> *mNodeList;
	int **InSDist;

	NodeDist **NodeToPortal;

private:
	bool HaveAdjMatrix;
	bool HaveNodeToPortal;

private:
	void ConstructAdjMatrix();
	void NodeListToAdjMatrix();
	void Floyd();
};




#endif