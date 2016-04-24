#ifndef _COSKQ_H
#define _COSKQ_H

#include <vector>
#include "ExtraInfo.h"
#include "TraverseInfo.h"

//CoSKQ query for customer limit max distance and max pairwise distance
void Rating_CoSKQ_Query(int StartNode, std::vector<KeyWord> KSet, std::pair<int, int> Cuslimit/*, int TopK*/);

//return the optimal rating score feasible set
void findOptimalSolution(int curlevel, int lastlevel, int PairDist, TraverseInfo *CliqueList);

//output statistic information as total running time
bool outputStatistic(int TotalTime);

#endif