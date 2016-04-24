#include <algorithm> //for sort invertList
#include <map> //for source to portal node
#include <windows.h> //for count time
#include "CoSKQ.h"
#include "TraverseInfo.h"
#include "macro.h"
#include "Global.h"

//opt result and rating
std::vector<int> *result;
int OptRating = 0;

//temp result and rating
std::vector<int> *desire;
int TempRating = 0;

//StartNodeToPortalNode
std::map<int, int> *StartToPortal;

//StartNodeDistance
int StartDist = 0;

bool RatingComp(int A, int B) {return NodeRating[A] > NodeRating[B];}

void Rating_CoSKQ_Query(int StartNode, std::vector<KeyWord> KSet, std::pair<int, int> Cuslimit, BlockPool& blockPool) {
	DWORD dwStart, dwStop;
	DWORD TotalTime = 0;

	desire = new std::vector<int>();

	OptRating = 0;

	for (int time = 0; time < 1; ++time) {

		std::vector<std::vector<int>> *invertList = new std::vector<std::vector<int>>(KSet.size(), std::vector<int>());
		result = new std::vector<int>();
		
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
		SearchIntraBlockIndex(StartNode, invertList, visited, KSet, Cuslimit.first, blockPool);

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
			TraverseInfo *CliqueList = new TraverseInfo(KSet.size(), invertList);

			ConstructClique(1, KSet.size(), Cuslimit.second, CliqueList, KSet, blockPool);

			delete CliqueList;

		}

		dwStop = GetTickCount();

		TotalTime += (dwStop - dwStart);

		delete invertList;
		delete result;

	}

	delete desire;
	
	OutPutResultToFile("Result\\TestResult.txt", OptRating);
}

void SearchIntraBlockIndex(int StartNode, std::vector<std::vector<int>> *invertList, bool *visited, std::vector<KeyWord>& KSet, int Farthest, BlockPool& blockPool) {
	
	//for each startnode's blockid
	int blockcount = (*BlockID)[StartNode].size();

	for (int BCount = 0; BCount < blockcount; ++BCount) {

		//find its intra block index
		int blockID = (*BlockID)[StartNode][BCount];

		//find intra block's nodeid
		int IntraID;

		//std::vector<int> *BlockNodes = ((*BlockArray)[blockID].mNodeList);
		Block* curBlock = blockPool.findBlock(blockID);
		
		std::vector<int> *BlockNodes = curBlock->mNodeList;

		int end = BlockNodes->size();
		for (int i = 0; i < end; ++i) {
			if (StartNode == (*BlockNodes)[i])
				IntraID = i;
		}

		//search intra block index
		//int **IntraIndex = (*BlockArray)[blockID].InSDist;

		int **IntraIndex = curBlock->InSDist;

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
		int endPortal = end - curBlock->mPNodePos;
		bool ThisUpdateIt[MAX_SWAP_SPACE] = { false }; //for avoid rep visited

		bool NotFeasiblePortal = false;

		//update source to portal nodes' shortest distance
		for (int portal = 0; !stop && portal < endPortal; ++portal) {
			
			int pdist = curBlock->NodeToPortal[IntraID][portal].Dist;

			if (pdist < Farthest) { //use portal node relative position in IntraIndex

				int pnode = curBlock->NodeToPortal[IntraID][portal].NodeID; //relative id

				pnode = (*(curBlock->mNodeList))[pnode]; //global id

				//search StartToPortal to pruning portal node
				std::map<int, int>::iterator rel = StartToPortal->find(pnode);

				int dist = StartDist + pdist; //cur dist

				if (rel != StartToPortal->end()) { //visited
					if (dist < rel->second) { //more shorter
						rel->second = dist;
						ThisUpdateIt[portal] = true;
					}
				}
				else { //unvisited
					StartToPortal->insert(std::pair<int, int>(pnode, dist)); //record
					ThisUpdateIt[portal] = true;
				}

			}
			else {
				stop = true;
				endPortal = portal; //!!!!
			}
		}

		//for each feasible portal node in this block in NodeToPortal list
		for (int portal = 0; portal < endPortal; ++portal) {

			int pnode = curBlock->NodeToPortal[IntraID][portal].NodeID; //relative id

			pnode = (*(curBlock->mNodeList))[pnode]; //global id

			//search StartToPortal to pruning portal node
			std::map<int, int>::iterator rel = StartToPortal->find(pnode);

			bool feasible = false;

			int pdist = curBlock->NodeToPortal[IntraID][portal].Dist;

			int dist = StartDist + pdist; //cur dist

			if (dist == rel->second && ThisUpdateIt[portal]) { //This update it and more shorter
				feasible = true;
			}

			if (feasible) {
				int CpyFarthest = Farthest;

				Farthest -= pdist;

				StartDist = dist;

				SearchIntraBlockIndex(pnode, invertList, visited, KSet, Farthest, blockPool); //IntraBlockSearch

				curBlock = blockPool.findBlock(blockID); //maybe curBlock didn't in mem after return back

				Farthest = CpyFarthest; //recover

				StartDist -= pdist;
			}
		}
	}
}

void ConstructClique(int curlevel, int lastlevel, int PairDist, TraverseInfo *CliqueList, std::vector<KeyWord>& KSet, BlockPool& blockPool) {
	
	if (curlevel != lastlevel) {
		
		//current max continuous keyword number
		int CurContinuous = CliqueList->Continuous[curlevel];

		//get relative candidate list
		std::vector<int> *CandList = CliqueList->f[CurContinuous][curlevel];

		//cal rating lower bound
		int RatingLower = 0;

		for (int kpos = curlevel + 1; kpos <= lastlevel; ++kpos) {
			RatingLower += NodeRating[(*CliqueList->f[curlevel - 1][kpos])[0]];
		}

		RatingLower = OptRating - RatingLower;

		bool RatingPruning = false;

		//for each nodes in Candidate list
		for (std::vector<int>::iterator itrClist = CandList->begin(); !RatingPruning && itrClist != CandList->end(); ++itrClist) {


			int curnodeid = *itrClist;

			//Rating score pruning
			if (TempRating + NodeRating[*itrClist] > RatingLower) {

				//construct NewCliqueList for keywords{level+1, ..., lastlevel}

				std::vector<std::vector<int>> *NewCliqueList = new std::vector<std::vector<int>>(lastlevel - curlevel, std::vector<int>());

				bool *visited = new bool[MAX_NODE_NUM];

				memset(visited, false, sizeof(bool)*MAX_NODE_NUM);

				visited[*itrClist] = true;

				StartToPortal = new std::map<int, int>();

				SearchIntraBlockIndex(*itrClist, NewCliqueList, visited, std::vector<KeyWord>(KSet.begin() + curlevel, KSet.end()), PairDist, blockPool);

				delete [] visited;
				delete StartToPortal;

				//f[curlevel - 1][curlevel + 1, ..., lastlevel] intersect NewCliqueList[0,...,] = f[curlevel][curlevel + 1, ..., lastlevel]

				bool NextCandNode = false;

				//for each f[...][curlevel+1, ..., lastlevel]; meanwhile NewCliqueList[0,...,]
				for (int IntersectCL = curlevel + 1, IntersectNewCL = 0; !NextCandNode && IntersectCL <= lastlevel; ++IntersectCL, ++IntersectNewCL) {

					//f intersect NewCliqueList
					std::vector<int> temp;

					//for each nodes in f[curlevel - 1][IntersectCL] 
					for (std::vector<int>::iterator CL = CliqueList->f[curlevel - 1][IntersectCL]->begin(); 
						CL != CliqueList->f[curlevel - 1][IntersectCL]->end(); ++CL) {

							bool stop = false;

							//test each nodes in NewCliqueList[...]
							for (std::vector<int>::iterator NewCL = (*NewCliqueList)[IntersectNewCL].begin(); 
								!stop && NewCL != (*NewCliqueList)[IntersectNewCL].end(); ++ NewCL) {

									if (*CL == *NewCL) {
										temp.push_back(*CL);
										stop = true;
									}

							}

					}

					if (!temp.empty()) {
						//= f[curlevel][curlevel + 1, ..., lastlevel]
						*(CliqueList->f[curlevel][IntersectCL]) = temp;

						//set continuous
						CliqueList->Continuous[IntersectCL] = curlevel;
					}
					else {
						NextCandNode = true;
					}
				}

				delete NewCliqueList;

				if (!NextCandNode) {

					//add node to desire array
					TempRating += NodeRating[*itrClist];
					desire->push_back(*itrClist);

					//search next keywords
					ConstructClique(curlevel + 1, lastlevel, PairDist, CliqueList, KSet, blockPool);

					//
					TempRating -= NodeRating[*itrClist];
					desire->pop_back();

					//modify continuous
					for (int level = curlevel + 1; level <= lastlevel; ++level) {
						if (CliqueList->Continuous[level] >= curlevel)
							--(CliqueList->Continuous[level]);
					}

				}

			}
			else
				RatingPruning = true;
		}
	}
	else { //last keyword

		std::vector<int> *LastCandList = CliqueList->f[lastlevel - 1][lastlevel];

		int Rate = TempRating + NodeRating[(*LastCandList)[0]];

		if (Rate > OptRating) {
			OptRating = Rate;
			*result = *desire;
			result->push_back((*LastCandList)[0]);
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