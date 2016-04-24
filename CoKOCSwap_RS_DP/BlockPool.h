#ifndef _BLOCKPOOL_H
#define _BLOCKPOOL_H

#include <windows.h> //for counting time
#include "Block.h" //definition before define!!!
#include "macro.h"


class BlockPool
{
public:
	BlockPool();
	~BlockPool();

	//Read BlockID to Mem
	bool ReadBlockIDtoMem(char filename[], int GraphNodeNum);

	//find block in pool or read it from disk
	Block* findBlock(int BlockID);

	//get FileIOTime
	DWORD getFileIOTime(void) { return FileIOTime; }

	//set fileiotime to zero
	void resetFileIOTime(void) { FileIOTime ^= FileIOTime; }

private:
	Block* ReadBlocktoPool(int BlockID);
	void AddBlock(Block* pblock);

private:
	int curBlockNum;
	Block* InMemBlock[TOTAL_BLOCK_INMEM];
	DWORD FileIOTime; //for counting the cost time of reading block from file
};




#endif