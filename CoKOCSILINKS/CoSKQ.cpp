#include <algorithm> //for sort invertList
#include <map> //for source to portal node
#include <windows.h> //for count time
#include "CoSKQ.h"
#include "Graph.h" //for NodeInfo and MaxNodeNum
#include "Global.h" //for block infor

//result
std::vector<std::vector<int>> *result;

//temp result and rating
std::vector<int> *desire;

//StartNodeToPortalNode
std::map<int, int> *StartToPortal;

//StartNodeDistance
int StartDist = 0;

int SMax[MAX_KEYWORD_NUM + 1] = {0};

bool RatingComp(int A, int B) {return NodeRating[A] > NodeRating[B];}

void Rating_CoSKQ_Query(int StartNode, std::vector<KeyWord> KSet, std::pair<int, int> Cuslimit) {
	DWORD dwStart, dwStop;
	DWORD TotalTime = 0;

	desire = new std::vector<int>();

	//for (int time = 0; time < 1; ++time) {

		std::vector<std::vector<int>> *invertList = new std::vector<std::vector<int>>(KSet.size(), std::vector<int>());
		result = new std::vector<std::vector<int>>();
		
		//construct inverted list in D(NodeID, MaxDist)

		int count = 0;
		bool only = true;

		dwStart = GetTickCount();

		//test StartNode whether it has keywords belong to KSet	`
		for (std::vector<KeyWord>::iterator itr = KSet.begin(); only && itr != KSet.end(); ++itr, ++count) {
			if (NodeKW[StartNode] & *itr) { //relevant with KSet
				(*invertList)[count].push_back(StartNode);
				only = false;
			}
		}

		//test Other Node

		bool *visited = new bool[MAX_NODE_NUM];

		memset(visited, false, sizeof(bool)*MAX_NODE_NUM);

		visited[StartNode] = true;

		StartToPortal = new std::map<int, int>();

		//for each startnode's blockid searchIntraBlockIndex
		SearchInvertList(StartNode, invertList, visited, KSet, Cuslimit.first);

		delete [] visited;
		delete StartToPortal;

		//test whether cover the keyword set
		bool cover = true;

		for (std::vector<std::vector<int>>::iterator itrK = invertList->begin(); cover && itrK != invertList->end(); ++itrK) {
			if (itrK->empty()) {
				cover = false;
			}
		}

		if (cover) {

			//sort invertList with Rating score
			for (std::vector<std::vector<int>>::iterator itr = invertList->begin(); itr != invertList->end(); ++itr) {
				std::sort(itr->begin(), itr->end(), RatingComp);
			}

			//find r-clique

			//trasvers info
			std::vector<int> *Candist = new std::vector<int>((*invertList)[0]);

			//find r-clique
			ConstructClique(0, KSet.size(), Cuslimit.second, invertList, Candist, KSet);

			delete Candist;

		}

		dwStop = GetTickCount();

		TotalTime += (dwStop - dwStart);

		OutPutResultToFile("Result\\TestResult.txt", result->size());

		delete invertList;
		delete result;

	//}

	delete desire;
	
	//OutPutResultToFile("Result\\TestResult.txt", TotalTime);
}

void SearchInvertList(int StartNode, std::vector<std::vector<int>> *invertList, bool *visited, std::vector<KeyWord>& KSet, int Farthest) {
	
	//for each startnode's blockid
	int blockcount = (*BlockID)[StartNode].size();

	for (int BCount = 0; BCount < blockcount; ++BCount) {

		//find its intra block index
		int blockID = (*BlockID)[StartNode][BCount];

		//find intra block's nodeid
		int IntraID;

		std::vector<int> *BlockNodes = ((*BlockArray)[blockID].mNodeList);
		int end = BlockNodes->size();
		for (int i = 0; i < end; ++i) {
			if (StartNode == (*BlockNodes)[i])
				IntraID = i;
		}

		//search intra block index
		int **IntraIndex = (*BlockArray)[blockID].InSDist;

		for (int node = 0; node < end; ++node) {

			int RealID = (*BlockNodes)[node];

			if (IntraIndex[IntraID][node] && IntraIndex[IntraID][node] <= Farthest && !visited[RealID]) {

				int count = 0; bool only = true;

				for (std::vector<KeyWord>::iterator itr = KSet.begin(); only && itr != KSet.end(); ++itr, ++count) {
					if (NodeKW[RealID] & *itr) {
						(*invertList)[count].push_back(RealID);
						only = false;
					}
				}

				visited[RealID] = true;
			}
		}

		//search outra block index

		bool stop = false;

		//for each portal node in this block in NodeToPortal list
		for (int portal = 0, endPortal = end - (*BlockArray)[blockID].mPNodePos; !stop && portal < endPortal; ++portal) {

			int pdist = (*BlockArray)[blockID].NodeToPortal[IntraID][portal].Dist;

			if (pdist) { //use portal node relative position in IntraIndex

				if (pdist < Farthest) {

					bool feasible = false;

					int pnode = (*BlockArray)[blockID].NodeToPortal[IntraID][portal].NodeID; //relative id

					pnode = (*(*BlockArray)[blockID].mNodeList)[pnode]; //global id

					//search StartToPortal to pruning portal node
					std::map<int, int>::iterator rel = StartToPortal->find(pnode);

					int dist = StartDist + pdist; //cur dist

					if (rel != StartToPortal->end()) { //visited

						if (dist < rel->second) { //more shorter
							feasible = true;
							rel->second = dist;
						}

					}
					else { //unvisited

						StartToPortal->insert(std::pair<int, int>(pnode, dist)); //record

						feasible = true;
					}

					if (feasible) {
						int CpyFarthest = Farthest;

						Farthest -= pdist;

						StartDist = dist;

						SearchInvertList(pnode, invertList, visited, KSet, Farthest); //IntraBlockSearch

						Farthest = CpyFarthest; //recover

						StartDist -= pdist;
					}

				}
				else
					stop = true;
				
			}
		}
	}
}

void SearchRClique(int StartNode, std::vector<int> *candidate, bool *visited, KeyWord keyword, int Farthest) {

	//for each startnode's blockid
	int blockcount = (*BlockID)[StartNode].size();

	for (int BCount = 0; BCount < blockcount; ++BCount) {

		//find its intra block index
		int blockID = (*BlockID)[StartNode][BCount];

		//find intra block's nodeid
		int IntraID;

		std::vector<int> *BlockNodes = ((*BlockArray)[blockID].mNodeList);
		int end = BlockNodes->size();
		for (int i = 0; i < end; ++i) {
			if (StartNode == (*BlockNodes)[i])
				IntraID = i;
		}

		//search intra block index
		int **IntraIndex = (*BlockArray)[blockID].InSDist;

		for (int node = 0; node < end; ++node) {

			int RealID = (*BlockNodes)[node];

			if (IntraIndex[IntraID][node] && IntraIndex[IntraID][node] <= Farthest && !visited[RealID]) {

				if (NodeKW[RealID] & keyword)
					candidate->push_back(RealID);

				visited[RealID] = true;
			}
		}

		//search outra block index

		bool stop = false;

		//for each portal node in this block in NodeToPortal list
		for (int portal = 0, endPortal = end - (*BlockArray)[blockID].mPNodePos; !stop && portal < endPortal; ++portal) {

			int pdist = (*BlockArray)[blockID].NodeToPortal[IntraID][portal].Dist;

			if (pdist) { //use portal node relative position in IntraIndex

				if (pdist < Farthest) {

					bool feasible = false;

					int pnode = (*BlockArray)[blockID].NodeToPortal[IntraID][portal].NodeID; //relative id

					pnode = (*(*BlockArray)[blockID].mNodeList)[pnode]; //global id

					//search StartToPortal to pruning portal node
					std::map<int, int>::iterator rel = StartToPortal->find(pnode);

					int dist = StartDist + pdist; //cur dist

					if (rel != StartToPortal->end()) { //visited

						if (dist < rel->second) { //more shorter
							feasible = true;
							rel->second = dist;
						}

					}
					else { //unvisited

						StartToPortal->insert(std::pair<int, int>(pnode, dist)); //record

						feasible = true;
					}

					if (feasible) {
						int CpyFarthest = Farthest;

						Farthest -= pdist;

						StartDist = dist;

						SearchRClique(pnode, candidate, visited, keyword, Farthest); //IntraBlockSearch

						Farthest = CpyFarthest; //recover

						StartDist -= pdist;
					}

				}
				else
					stop = true;

			}
		}
	}
}

void ConstructClique(int curlevel, int lastlevel, int PairDist, std::vector<std::vector<int>> *invertList, std::vector<int> *CandList, std::vector<KeyWord>& KSet) {

	if (curlevel != lastlevel - 1) {

		//for each nodes in Candidate list
		for (std::vector<int>::iterator itrClist = CandList->begin(); itrClist != CandList->end(); ++itrClist) {

			//add node to desire array
			desire->push_back(*itrClist);

			//construct NextCandList for keywords{level+1}

			std::vector<int> *NextCandList = new std::vector<int>((*invertList)[curlevel + 1]);

			for (std::vector<int>::iterator kpos = desire->begin(); kpos != desire->end(); ++kpos) {

				std::vector<int> *NewCliqueList = new std::vector<int>();

				bool *visited = new bool[MAX_NODE_NUM];

				memset(visited, false, sizeof(bool)*MAX_NODE_NUM);

				visited[*kpos] = true;

				StartToPortal = new std::map<int, int>();

				SearchRClique(*kpos, NewCliqueList, visited, KSet[curlevel + 1], PairDist);

				delete [] visited;
				delete StartToPortal;

				std::vector<int> *temp = new std::vector<int>();

				//NextCandList intersect NewCliqueList = NextCandList
				for (std::vector<int>::iterator itrIL = NextCandList->begin(); itrIL != NextCandList->end(); ++itrIL) {

					bool same = false;

					for (std::vector<int>::iterator itrNCL = NewCliqueList->begin(); !same && itrNCL != NewCliqueList->end(); ++itrNCL) {

						if (*itrIL == *itrNCL)
							same = true;

					}

					if (same)
						temp->push_back(*itrIL);
				}

				*NextCandList = *temp;

				delete NewCliqueList;
				delete temp;

			}

			if (!NextCandList ->empty()) {

				//search next keywords
				ConstructClique(curlevel + 1, lastlevel, PairDist, invertList, NextCandList, KSet);

			}

			desire->pop_back();

			delete NextCandList;
		}
	}
	else { //last keyword

		//there makes difference

		for (std::vector<int>::iterator itr = CandList->begin(); itr != CandList->end(); ++itr) {

			desire->push_back(*itr);

			result->push_back(*desire);

			desire->pop_back();

		}
	}
}

void OutPutResultToFile(char filename[], int Time) {
	FILE *file = fopen(filename, "a");

	if (file == NULL)
		return;

	fprintf(file, "%d\n", Time);

	fclose(file);
}