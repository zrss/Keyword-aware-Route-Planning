#ifndef _COSKQ_H
#define _COSKQ_H

#include <vector>
#include "ExtraInfo.h"
#include "TraverseInfo.h"
#include "BlockPool.h"

//CoSKQ query for customer limit max distance and max pairwise distance
void Rating_CoSKQ_Query(int StartNode, std::vector<KeyWord> KSet, std::pair<int, int> Cuslimit, BlockPool& blockPool);

//first parameter start node; invertList; visited array, Kset, farthest distance, transfer block flag array
void SearchIntraBlockIndex(int, std::vector<std::vector<int>> *, bool *, std::vector<KeyWord>&, int, BlockPool&);

//first parameter which level (keyword); lastlevel (keyword); PairDist(r-clique); TraverseInfo; query keyword set
void ConstructClique(int, int, int, TraverseInfo*, std::vector<KeyWord>&, BlockPool&);

//OutPutResultToFile
void OutPutResultToFile(char filename[], int Time);

#endif