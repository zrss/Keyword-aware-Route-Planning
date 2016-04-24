#ifndef _SDIST_H
#define _SDIST_H

#include <string>

#define MAX_NODE_NUM 20000

extern int SDist[MAX_NODE_NUM][MAX_NODE_NUM];

//node labled 1...n
//NodeNum = Real Node Number + 1.
//get adjacent matrix
//format as "a %d %d %d"
bool getAdjMatrix(std::string filename, int NodeNum);

//Real Node Number
//floyd algorithm calculate every two nodes' shortest dist
void Floyd(int NodeNum);

//Real Node Number
//output shortest dist matrix
bool outputSD(std::string filename, int NodeNum);

//Real Node Number
//get shortest dist matrix
bool getSDMatrix(std::string filename, int NodeNum);

//calculate 5000, 10000, 15000, 20000 node number shortest dist matrix
void generateSD(void);

#endif