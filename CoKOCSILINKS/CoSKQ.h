#ifndef _COSKQ_H
#define _COSKQ_H

#include <vector>
#include "ExtraInfo.h"

//CoSKQ query for customer limit max distance and max pairwise distance
void Rating_CoSKQ_Query(int StartNode, std::vector<KeyWord> KSet, std::pair<int, int> Cuslimit);

//first parameter start node; invertList; visited array, Kset, farthest distance, transfer block flag array
void SearchInvertList(int, std::vector<std::vector<int>> *, bool *, std::vector<KeyWord>&, int);

void ConstructClique(int, int, int, std::vector<std::vector<int>>*, std::vector<int>*, std::vector<KeyWord>&);

void SearchRClique(int StartNode, std::vector<int> *candidate, bool *visited, KeyWord keyword, int Farthest);

//OutPutResultToFile
void OutPutResultToFile(char filename[], int Time);

#endif