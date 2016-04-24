#include <algorithm>
#include "CoSKQ.h"

std::vector<int> *result;
std::vector<int> *desire;

int OptRating = 0;
int CurrentRating = 0;

int SMax[MAX_KEYWORD_NUM + 2] = {0};

//inverted list
std::vector<std::vector<NodeInfo>> *invertList;

void Rating_CoSKQ_Query(int StartNode, std::vector<KeyWord> KSet, std::pair<int, int> Cuslimit) {
	result = new std::vector<int>();
	desire = new std::vector<int>();
	invertList = new std::vector<std::vector<NodeInfo>>(KSet.size(), std::vector<NodeInfo>());

	OptRating = 0;
	CurrentRating = 0;

	//construct inverted list in D(NodeID, MaxDist)
	int count = 0;
	bool only = true;

	//test StartNode whether it has keywords belong to KSet	`
	for (std::vector<KeyWord>::iterator itr = KSet.begin(); only && itr != KSet.end(); ++itr, ++count) {
		if (NodeKW[StartNode] & *itr) { //relevant with KSet
			(*invertList)[count].push_back(NodeInfo(StartNode, NodeRating[StartNode]));
			only = false;
		}
	}

	//test Other Node
	for (int i = 0; i < MAX_NODE_NUM; ++i) {
		if (SDist[StartNode][i] && SDist[StartNode][i] <= Cuslimit.first) {
			count = 0; only = true;
			for (std::vector<KeyWord>::iterator itr = KSet.begin(); only && itr != KSet.end(); ++itr, ++count) {
				if (NodeKW[i] & *itr) {
					(*invertList)[count].push_back(NodeInfo(i, NodeRating[i]));
					only = false;
				}
			}
		}
	}

	bool cover = true;

	//test whether cover the keyword set
	for (std::vector<std::vector<NodeInfo>>::iterator itrK = (*invertList).begin(); cover && itrK != (*invertList).end(); ++itrK) {
		if (itrK->empty()) {
			cover = false;
		}
	}

	if (cover) {
		//sorted inverted list rank with the rating score
		for (std::vector<std::vector<NodeInfo>>::iterator itrK = (*invertList).begin(); itrK != (*invertList).end(); ++itrK) {
			std::sort((*itrK).begin(), (*itrK).end());
		}

		//construct SMax
		count = KSet.size() - 1;
		int sum = 0;
		for (std::vector<std::vector<NodeInfo>>::reverse_iterator ritrK = invertList->rbegin(); ritrK != invertList->rend(); ++ritrK) {
			sum += ((ritrK->begin()))->mRating;
			SMax[count--] = sum;
		}

		//find a feasible set
		findOptimalSolution(0, KSet.size(), Cuslimit.second);
	}
	
	//Output result
	//outputStatistic("Result\\TestResult.txt", OptRating);

	outputNodeList("Result\\TestResult.txt");

	delete invertList;
	delete desire;
	delete result;
}

void findOptimalSolution(int Kpos, int endOfpos, int curMaxPair) {
	int RatingLower = OptRating - SMax[Kpos + 1];
	bool cont = true;

	for (std::vector<NodeInfo>::iterator itrIL = (*invertList)[Kpos].begin(); cont && itrIL != (*invertList)[Kpos].end(); ++itrIL) {
		CurrentRating += (*itrIL).mRating;

		if (CurrentRating > RatingLower) {
			bool correct = true;

			for (std::vector<int>::iterator itrNode = desire->begin(); correct && itrNode != desire->end(); ++itrNode) {
				if (SDist[*itrNode][(*itrIL).mID] > curMaxPair) {
					correct = false;
				}
			}

			if (correct) {
				desire->push_back((*itrIL).mID);

				if (Kpos + 1 != endOfpos) {
					findOptimalSolution(Kpos + 1, endOfpos, curMaxPair);

					desire->pop_back();
				}
				else { //the last keyword
					if (CurrentRating > OptRating) {
						OptRating = CurrentRating;
						*result = *desire;
					}

					desire->pop_back();

					CurrentRating -= (*itrIL).mRating;

					return;
				}
			}
		}
		else
			cont = false;

		CurrentRating -= (*itrIL).mRating;
	}

	return;
}

void outputStatistic(char filename[], int Data) {
	FILE *file = fopen(filename, "a");

	if (file == NULL)
		return;

	fprintf(file, "%d\n", Data);

	fclose(file);
}

void outputNodeList(char filename[]) {
	FILE *file = fopen(filename, "a");

	if (file == NULL)
		return;

	for (std::vector<int>::iterator itr = result->begin(); itr != result->end(); ++itr) {
		fprintf(file, "%d ", *itr);
	}

	fprintf(file, "\n");

	fclose(file);
}