#ifndef _BLOCK_H
#define _BLOCK_H

#include <vector>
#include "macro.h" //Max_swap_space
#include "NodeDist.h" //for nodedist struct for node to portal index

class Block
{
public:
	Block();
	Block(int ID, int PnodePos, int NodeNum, int *NodeList); //construct Nodelist array
	Block(const Block& cpy);
	~Block();

	void ConstructNodeToPortal(NodeDist matrix[MAX_SWAP_SPACE][MAX_SWAP_SPACE / 10]);
	void ConstructAdjMatrix(int matrix[MAX_SWAP_SPACE][MAX_SWAP_SPACE]);

	void release(void);

	int mID;
	int mNodeNum; //pure intra nodes + portal nodes
	int mPNodePos; //the number of node excepte portal nodes
	
	std::vector<int> *mNodeList;
	int **InSDist;

	NodeDist **NodeToPortal;
};




#endif