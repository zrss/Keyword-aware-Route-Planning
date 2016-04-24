#ifndef _PARTITION_H
#define _PARTITION_H

#include <queue> //for bfs
#include <vector> //for store block
#include <list> //for store Partition Edges
#include "Graph.h" //for adjmatrix
#include "Block.h" //for block class

class Partition
{
public:
	Partition(int GraphNodeNum);
	~Partition();

	//partition graph to blocks
	void ConstructBlock(int StartNode, int BlockSize);

	//find block portal nodes
	void findBlockPortalNodes();

	//ConstructIntraBlockIndex
	void ConstructIntraBlockIndex();

	//OutPut Partition Infor
	bool OutPutPartitionInfor(char filename[]);

private:
	//bfs method to construct block
	int BFS(int StartNode, int BlockSize, int curBlockID, int NodeList[MAX_BLOCK_NODE]);

	//find partition edges; store in PEdgeArray
	void findPartitionEdges(std::vector<std::list<int>> &PEdgeArray);

private:
	bool *visited; //visited flag array for contruct block
};



#endif