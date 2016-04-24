#ifndef _GRAPH_
#define _GRAPH_


#define MAX_NODE_NUM 20000

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

extern int **AdjMatrix;

bool getPraticalGraph(char filename[], int NodeNum);

bool getAdjMatrix(char filename[], int NodeNum);

void ConstructGlobalMatrix(int NodeNum);

void ReleaseGlobalMatrix(int NodeNum);


#endif