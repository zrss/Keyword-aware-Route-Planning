#ifndef _COSKQ_H
#define _COSKQ_H

#include <vector>
#include <set>
#include "ExtraInfo.h"

extern std::vector<int> *result; //maybe multiple results
extern std::vector<int> *desire; //temp result

//CoSKQ query for customer limit max distance and max pairwise distance
void Rating_CoSKQ_Query(int StartNode, std::vector<KeyWord> KSet, std::pair<int, int> Cuslimit);

//return the optimal rating score feasible set
void findOptimalSolution(int Kpos, int endOfpos, int curMaxPair);

//output statistic information as total running time and result number
void outputStatistic(char filename[], int Data);

//output NodeList
void outputNodeList(char filename[]);

#endif