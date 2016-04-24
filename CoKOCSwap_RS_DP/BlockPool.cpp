#include "BlockPool.h"

std::vector<std::vector<int>> *BlockID = NULL; //each nodes' blockID

int nodelist[MAX_SWAP_SPACE]; //temp store reading nodelist
int adjmatrix[MAX_SWAP_SPACE][MAX_SWAP_SPACE]; //temp store reading intrablock
NodeDist nodeToPortal[MAX_SWAP_SPACE][MAX_SWAP_SPACE / 10]; //temp store reading nodetoportal

BlockPool::BlockPool() :curBlockNum(0), FileIOTime(0) {
	for (int i = 0; i < TOTAL_BLOCK_INMEM; ++i) {
		InMemBlock[i] = NULL;
	}
}

void BlockPool::AddBlock(Block* pblock) {
	//if have a free pos
	if (curBlockNum != TOTAL_BLOCK_INMEM) {
		InMemBlock[curBlockNum] = pblock;
		++curBlockNum;
	}
	else {
		//find the minimum block in mem
		int minSize = MAX_TOTAL_BLOCK_NODE;
		int pos = 0;

		for (int i = 0; i < TOTAL_BLOCK_INMEM; ++i) {
			if (InMemBlock[i]->mNodeNum < minSize) {
				minSize = InMemBlock[i]->mNodeNum;
				pos = i;
			}
		}

		//Release this pos block 
		delete InMemBlock[pos];
		InMemBlock[pos] = NULL;

		//then add cur pblock
		InMemBlock[pos] = pblock;
	}
}

Block* BlockPool::findBlock(int BlockID) {
	
	//find in blockPool
	for (int i = 0; i < curBlockNum; ++i) {
		if (InMemBlock[i]->mID == BlockID)
			return InMemBlock[i];
	}

	//read from disk
	return ReadBlocktoPool(BlockID);
}

Block* BlockPool::ReadBlocktoPool(int BlockID) {

	//construct intput name
	char filename[20] = "Index\\Block_";
	char charBlockID[10] = { '\0' };

	sprintf(charBlockID, "%d", BlockID);

	int len = strlen(charBlockID);

	for (int cat = 0; cat < len; ++cat) {
		filename[cat + 12] = charBlockID[cat];
	}

	filename[len + 12] = '\0';
	//

	DWORD dwStart, dwStop;

	dwStart = GetTickCount();

	FILE *InFile = fopen(filename, "r");

	if (InFile == NULL)
		return NULL;

	//read
	int id, mpnodepos, totalnode;
	fscanf(InFile, "%d %d %d\n", &id, &mpnodepos, &totalnode);

	//read
	for (int node = 0; node < totalnode; ++node) {
		fscanf(InFile, "%d ", nodelist + node);
	}

	//read
	for (int i = 0; i < totalnode; ++i) {
		for (int j = 0; j < totalnode; ++j) {
			fscanf(InFile, "%d ", &adjmatrix[i][j]);
		}
	}

	//read
	int PortalCount = totalnode - mpnodepos;

	for (int i = 0; i < totalnode; ++i) {
		for (int j = 0; j < PortalCount; ++j) {
			fscanf(InFile, "%d %d ", &nodeToPortal[i][j].NodeID, &nodeToPortal[i][j].Dist);
		}
	}

	dwStop = GetTickCount();

	FileIOTime += (dwStop - dwStart);

	//construct block
	Block *block = new Block(id, mpnodepos, totalnode, nodelist); 

	block->ConstructAdjMatrix(adjmatrix);

	block->ConstructNodeToPortal(nodeToPortal);
	//

	AddBlock(block); //add block to blockPool

	fclose(InFile); //alert you to shutdown infile !!!!!!!

	return block;
}

bool BlockPool::ReadBlockIDtoMem(char filename[], int GraphNodeNum) {
	FILE *InFile = fopen(filename, "r");

	if (InFile == NULL)
		return false;

	BlockID = new std::vector<std::vector<int>>(GraphNodeNum, std::vector<int>());

	for (int node = 0; node < GraphNodeNum; ++node) {

		int len;
		fscanf(InFile, "%d ", &len);

		for (int i = 0; i < len; ++i) {
			int temp;
			fscanf(InFile, "%d ", &temp);

			(*BlockID)[node].push_back(temp);
		}

	}

	fclose(InFile);

	return true;
}

BlockPool::~BlockPool() {
	for (int i = 0; i < TOTAL_BLOCK_INMEM; ++i) {
		if (InMemBlock[i] != NULL)
			delete InMemBlock[i];
	}

	if (BlockID != NULL)
		delete BlockID;
}