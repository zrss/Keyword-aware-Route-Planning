#include <algorithm>
#include "windows.h"
#include "CoSKQ.h"
#include "TopKResult.h"

//TopKResult *topKResult;

std::vector<int> *result;
int OptRating;

std::vector<int> *desire;
int TempRating = 0;

bool RatingComp(int A, int B) {return NodeRating[A] > NodeRating[B];}

void Rating_CoSKQ_Query(int StartNode, std::vector<KeyWord> KSet, std::pair<int, int> Cuslimit/*, int TopK*/) {
	DWORD dwStart, dwStop;
	int TotalTime = 0;

	desire = new std::vector<int>();

	//topKResult = new TopKResult(TopK, KSet.size());

	for (int time = 0; time < 10000; ++time) {

		result = new std::vector<int>();
		OptRating = 0;

		//construct inverted list in D(NodeID, MaxDist)
		int count = 1;
		bool only = true;

		TraverseInfo *traverseInfo = new TraverseInfo(KSet.size());

		//f[0, 1], f[0, 2], f[0, ...] store previous invertList

		dwStart = GetTickCount();

		//test StartNode whether it has keywords belong to KSet	`
		for (std::vector<KeyWord>::iterator itr = KSet.begin(); only && itr != KSet.end(); ++itr, ++count) {
			if (NodeKW[StartNode] & *itr) { //relevant with KSet
				traverseInfo->f[0][count]->push_back(StartNode);
				only = false;
			}
		}

		//test Other Node
		for (int i = 0; i < MAX_NODE_NUM; ++i) {
			if (SDist[StartNode][i] && SDist[StartNode][i] <= Cuslimit.first) {

				count = 1; only = true;

				for (std::vector<KeyWord>::iterator itr = KSet.begin(); only && itr != KSet.end(); ++itr, ++count) {
					if (NodeKW[i] & *itr) {
						traverseInfo->f[0][count]->push_back(i);
						only = false;
					}
				}
			}
		}

		bool cover = true;

		//test whether cover the keyword set
		for (int k = 1, KeywordNum = KSet.size(); cover && k <= KeywordNum; ++k) {
			if (traverseInfo->f[0][k]->empty())
				cover = false;
		}

		if (cover) {

			//sorted inverted list rank with the rating score
			for (int k = 1, KeywordNum = KSet.size(); k <= KeywordNum; ++k) {
				std::sort(traverseInfo->f[0][k]->begin(), traverseInfo->f[0][k]->end(), RatingComp);
			}

			//find a feasible set
			findOptimalSolution(1, KSet.size(), Cuslimit.second, traverseInfo);

		}

		dwStop = GetTickCount();

		TotalTime += (dwStop - dwStart);

		//topKResult->OutPutResultToFile("Result\\TestResult.txt");

		delete traverseInfo;
		delete result;
		//delete topKResult;
	}
	
	outputStatistic(TotalTime);
	
	
	delete desire;
	
}

void findOptimalSolution(int curlevel, int lastlevel, int PairDist, TraverseInfo *CliqueList) {

	//current max continuous keyword number
	int CurContinuous = CliqueList->Continuous[curlevel];

	//get relative candidate list
	std::vector<int> *CandList = CliqueList->f[CurContinuous][curlevel];

	//cal rating lower bound
	int RatingLower = 0;

	for (int kpos = curlevel + 1; kpos <= lastlevel; ++kpos) {
		RatingLower += NodeRating[(*CliqueList->f[curlevel - 1][kpos])[0]];
	}

	RatingLower = OptRating/*topKResult->getMinRating()*/ - RatingLower;

	bool RatingPruning = false;

	//for each nodes in Candidate list
	for (std::vector<int>::iterator itrClist = CandList->begin(); !RatingPruning && itrClist != CandList->end(); ++itrClist) {

		//Rating score pruning
		if (TempRating + NodeRating[*itrClist] > RatingLower) {

			//construct CliqueList for keywords{level+1, ..., lastlevel}

			//f[curlevel - 1][curlevel + 1, ..., lastlevel] test with current candidate node = f[curlevel][curlevel + 1, ..., lastlevel]
			int prelevel = curlevel - 1;
			bool NextCandNode = true;

			int k;

			for (k = curlevel + 1; NextCandNode && k <= lastlevel; ++k) {
				for (std::vector<int>::iterator itr = CliqueList->f[prelevel][k]->begin(); itr != CliqueList->f[prelevel][k]->end(); ++itr) {
					if (SDist[*itrClist][*itr] <= PairDist)
						CliqueList->f[curlevel][k]->push_back(*itr);
				}

				if (CliqueList->f[curlevel][k]->empty())
					NextCandNode = false;
			}

			if (NextCandNode) {

				for (int i = curlevel + 1; i <= lastlevel; ++i) {
					CliqueList->Continuous[i] = curlevel;
				}

				//add node to desire array
				TempRating += NodeRating[*itrClist];
				desire->push_back(*itrClist);

				if (curlevel != lastlevel - 1) { //search next keywords
					findOptimalSolution(curlevel + 1, lastlevel, PairDist, CliqueList);
				}
				else {

					std::vector<int> *LastCandList = CliqueList->f[lastlevel - 1][lastlevel];

					int Rate = TempRating + NodeRating[(*LastCandList)[0]];

					if (Rate > OptRating) {
						OptRating = Rate;
						*result = *desire;
						result->push_back((*LastCandList)[0]);
					}

/*
					desire->push_back((*LastCandList)[0]);

					topKResult->insert(*desire, Rate);

					desire->pop_back();*/

				}

				//
				TempRating -= NodeRating[*itrClist];
				desire->pop_back();

				//modify continuous and clear unvalid traverseinfo
				for (int level = curlevel + 1; level <= lastlevel; ++level) {
					if (CliqueList->Continuous[level] >= curlevel) {
						CliqueList->f[CliqueList->Continuous[level]][level]->clear();
						--(CliqueList->Continuous[level]);
					}
				}
			}
			else {
				for (int perK = curlevel + 1; perK < k; ++perK) {
					CliqueList->f[curlevel][perK]->clear();
				}
			}

		}
		else
			RatingPruning = true;
	}
}

bool outputStatistic(int TotalTime) {
	FILE* StaFile = fopen("Result\\TestResult.txt", "a");

	if (NULL == StaFile)
		return false;

	fprintf(StaFile, "%d\n", TotalTime);

	fclose(StaFile);
	return true;
}