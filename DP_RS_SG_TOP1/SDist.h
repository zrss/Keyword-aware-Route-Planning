#ifndef _SDIST_H
#define _SDIST_H

#include <string>

#define MAX_NODE_NUM 20000

extern int SDist[MAX_NODE_NUM][MAX_NODE_NUM];

//Graph Node information ID and Rating score
struct NodeInfo {
	NodeInfo():mID(0), mRating(0) {};
	NodeInfo(int ID, int Dist):mID(ID), mRating(Dist) {};
	NodeInfo(const NodeInfo& C) {mID = C.mID; mRating = C.mRating;}

	int mID;
	int mRating;

	bool operator< (const NodeInfo& rt) const {
		return this->mRating > rt.mRating;
	}
};

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