#include "Partition.h"

std::vector<std::vector<int>> *BlockID; //each nodes' blockID
std::vector<Block> *BlockArray; //store blocks


Partition::Partition(int GraphNodeNum) {
	visited = new bool[GraphNodeNum];
	memset(visited, false, sizeof(bool)*GraphNodeNum);

	BlockID = new std::vector<std::vector<int>>(GraphNodeNum, std::vector<int>(1, 0));

	BlockArray = new std::vector<Block>();
}

Partition::~Partition()
{
	delete [] visited;

	delete BlockID;

	delete BlockArray;
}

void Partition::ConstructBlock(int StartNode, int BlockSize) {
	int curBlockID = 0;

	int NodeList[MAX_BLOCK_NODE]; //store each block's node id

	//center at startnode bfs
	int blockSize = BFS(StartNode, BlockSize, curBlockID, NodeList);

	BlockArray->push_back(Block(curBlockID, blockSize, NodeList));

	++curBlockID;

	//select next unvisited node to construct next block
	for (int i = 0; i < MAX_NODE_NUM; ++i) {
		if (!visited[i]) {
			int blockSize = BFS(i, BlockSize, curBlockID, NodeList);

			BlockArray->push_back(Block(curBlockID, blockSize, NodeList));

			++curBlockID;
		}
	}
}

int Partition::BFS(int StartNode, int BlockSize, int curBlockID, int NodeList[MAX_BLOCK_NODE]) {

	//if startnode have been visited return
	if (visited[StartNode])
		return 0;

	//queue for support BFS
	std::queue<int> myQueue;

	//blockSize
	int curBlockSize = 0;

	//whether reach the demanded blocksize flag
	bool enough = false;

	//current node enque
	myQueue.push(StartNode);

	NodeList[0] = StartNode; 

	//set startnode visited flag to be true
	visited[StartNode] = true;

	//blockid = curBlockID
	(*BlockID)[StartNode][0] = curBlockID;

	//addup blocksize
	++curBlockSize;

	while (!myQueue.empty() && !enough)
	{
		//current extented node
		int curNode = myQueue.front();

		//test adjacent node
		for (int i = curNode + 1; !enough && i < MAX_NODE_NUM; ++i) {

			//didn't visited and have a path from curNode to i
			if (!visited[i] && AdjMatrix[curNode][i]) {

				myQueue.push(i);

				//set visited flag to true
				visited[i] = true;

				//add to block[curBlockID]
				(*BlockID)[i][0] = curBlockID;

				NodeList[curBlockSize] = i;

				//add up curBlockSize
				++curBlockSize;

				//reach demanded blocksize
				if (curBlockSize == BlockSize)
					enough = true;
			}
		}

		//current extented node outque
		myQueue.pop();
	}

	return curBlockSize;
}

void Partition::findPartitionEdges(std::vector<std::list<int>> &PEdgeArray) {
	for (int i = 0; i < MAX_NODE_NUM; ++i) {
		for (int j = 0; j < MAX_NODE_NUM; ++j) {
			if (AdjMatrix[i][j] && (*BlockID)[i][0] != (*BlockID)[j][0]) {
				PEdgeArray[i].push_back(j);
			}
		}
	}
}

void Partition::findBlockPortalNodes() {
	std::vector<std::list<int>> PEdgeArray(MAX_NODE_NUM, std::list<int>());

	//find partition edges of the graph
	findPartitionEdges(PEdgeArray);

	//use partition edges to find portal nodes

	bool BePotalNode[MAX_NODE_NUM] = {false};

	//for each node in Graph
	for (int i = 0; i < MAX_NODE_NUM; ++i) {

		//modify Si
		for (std::list<int>::iterator itr = PEdgeArray[i].begin(); itr != PEdgeArray[i].end(); ) {
			if (BePotalNode[*itr])
				itr = PEdgeArray[i].erase(itr);
			else
				++itr;
		}

		int Si = PEdgeArray[i].size(); //the number of incident partition edges

		if (Si) {

			int BSi = (*BlockArray)[(*BlockID)[i][0]].getBlockSize(); //the size of the block with node i

			bool Stop = false;

			for (std::list<int>::iterator itr = PEdgeArray[i].begin(); !Stop && itr != PEdgeArray[i].end(); ++itr) {

				//modify Sj 
				for (std::list<int>::iterator itrSj = PEdgeArray[*itr].begin(); itrSj != PEdgeArray[*itr].end(); ) {
					if (BePotalNode[*itrSj])
						itrSj = PEdgeArray[*itr].erase(itrSj);
					else
						++itrSj;
				}

				int Sj = PEdgeArray[*itr].size();

				if (Sj) {

					int BSj = (*BlockArray)[(*BlockID)[*itr][0]].getBlockSize();

					if (Si + BSi > Sj + BSj) {
						Stop = true;
						BePotalNode[i] = true;

						//portal node add to all adjblock
						for (std::list<int>::iterator itrAdj = PEdgeArray[i].begin(); 
							itrAdj != PEdgeArray[i].end(); ++itrAdj) {

								int blockID = (*BlockID)[*itrAdj][0];

								if (!(*BlockArray)[blockID].TestRepetition(i)) {

									(*BlockArray)[blockID].AddNode(i);

									(*BlockID)[i].push_back(blockID);
								}

						}

						//portal node add to itselfs block
						(*BlockArray)[(*BlockID)[i][0]].AddNode(i);
					}
					else {
						BePotalNode[*itr] = true;

						//add to all adjblock
						for (std::list<int>::iterator itrAdj = PEdgeArray[*itr].begin(); 
							itrAdj != PEdgeArray[*itr].end(); ++itrAdj) {

								int blockID = (*BlockID)[*itrAdj][0];

								if (!(*BlockArray)[blockID].TestRepetition(*itr)) {

									(*BlockArray)[blockID].AddNode(*itr);

									(*BlockID)[*itr].push_back(blockID);
								}
						}

						//portal node add to itselfs block
						(*BlockArray)[(*BlockID)[*itr][0]].AddNode(*itr);
						
						//modify node i's Si and BSi
						//--Si;
						//++BSi;

						//as have no influence to Si + BSi

						PEdgeArray[*itr].clear();
					}

				}
			}
		}
	}

	return;
}

void Partition::ConstructIntraBlockIndex() {
	for (std::vector<Block>::iterator itr = BlockArray->begin(); itr != BlockArray->end(); ++itr) {
		itr->CalIntraBlockIndex();
		itr->CalNodeToPortalIndex();
	}
}

bool Partition::OutPutPartitionInfor(char filename[]) {
	FILE *OutFile = fopen(filename, "w");

	if (OutFile == NULL)
		return false;

	//the number of block
	fprintf(OutFile, "the number of block is %d.\n", BlockArray->size());

	//portal nodes
	for (int Node = 0; Node < MAX_NODE_NUM; ++Node) {
		if ((*BlockID)[Node].size() > 1)
			fprintf(OutFile, "NodeID: %d. AdjBlockNumber: %d.\n", Node, (*BlockID)[Node].size());
	}

	return true;
}
