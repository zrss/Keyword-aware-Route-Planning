#include <algorithm>
#include "windows.h"
#include "CoSKQ.h"

//TopKResult *topKResult;

std::vector<int> *result;
int OptRating;

std::vector<int> *desire;
int CurrentRating = 0;

//int SMax[MAX_KEYWORD_NUM + 2] = {0}; // +2 for final keyword

//inverted list
std::vector<std::vector<NodeInfo>> *invertList;

//xpos end
int EndStartPos[MAX_KEYWORD_NUM + 1] = {0};

//max continuous
int Continuous[MAX_KEYWORD_NUM + 1] = {0};

//flag, first element is the size
int flag[MAX_KEYWORD_NUM + 1][MAX_KEYWORD_NUM + 1][500] = {0};

//last time visit flag
int lastPosFlag[MAX_KEYWORD_NUM + 1][MAX_KEYWORD_NUM + 1] = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

void Rating_CoSKQ_Query(int StartNode, std::vector<KeyWord> KSet, std::pair<int, int> Cuslimit) {
	result = new std::vector<int>();
	desire = new std::vector<int>();
	//CurrentRating = 0;

	//topKResult = new TopKResult(TopK, KSet.size());

	OptRating = 0;

	//construct inverted list in D(NodeID, MaxDist)
	int count = 1;
	bool only = true;

	invertList = new std::vector<std::vector<NodeInfo>>(KSet.size() + 1, std::vector<NodeInfo>());

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
			count = 1; only = true;
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
	for (std::vector<std::vector<NodeInfo>>::iterator itrK = ++(*invertList).begin(); cover && itrK != (*invertList).end(); ++itrK) {
		if (itrK->empty()) {
			cover = false;
		}
	}

	if (cover) {

		//initial EndOfPos
		EndStartPos[1] = 0;

		//sorted inverted list rank with the rating score
		for (std::vector<std::vector<NodeInfo>>::iterator itrK = ++(*invertList).begin(); itrK != (*invertList).end(); ++itrK) {
			std::sort((*itrK).begin(), (*itrK).end());
		}

		//construct SMax
/*
		count = KSet.size();
		int sum = 0;
		std::vector<std::vector<NodeInfo>>::reverse_iterator rend = --invertList->rend();

		for (std::vector<std::vector<NodeInfo>>::reverse_iterator ritrK = invertList->rbegin(); ritrK != rend; ++ritrK) {
			sum += ((ritrK->begin()))->mRating;
			SMax[count] = sum;
			--count;
		}*/

		//find a feasible set
		findOptimalSolution(1, KSet.size(), Cuslimit.second);

	}

	//topKResult->OutPutResultToFile("Result\\TestResult.txt");

	outputStatistic(OptRating);
	
	delete invertList;
	delete desire;
	//delete topKResult;
	delete result;
}

void findOptimalSolution(int Kpos, int endOfpos, int curMaxPair) {

/*
	bool RatingPrunning = false;

	int Ratinglower = / *topKResult->getMinRating()* / OptRating - SMax[Kpos + 1];*/

	int NodeNum = (*invertList)[Kpos].size();

	while (/*!RatingPrunning && */(EndStartPos[Kpos] !=  NodeNum || (Kpos > 2 && lastPosFlag[Continuous[Kpos]][Kpos] != (flag[Continuous[Kpos]][Kpos][0] + 1)))) {

		//test pair distance limit

		//use pre pair distance limit info

		int endOfdesire = desire->size();

		int cur = Continuous[Kpos];

		int candSize = flag[cur][Kpos][0];

		bool Precorrect = false;

		if (candSize) { 
			
			int now;

			for (now = lastPosFlag[cur][Kpos]; !Precorrect && now <= candSize; ++now) {
				
				Precorrect = true;

				for (int start = cur; Precorrect && start < endOfdesire; ++start) {
					if (SDist[flag[cur][Kpos][now]][(*desire)[start]] > curMaxPair) {
						Precorrect = false;

						//avoid repitition
						//move to flag[start][Kpos]
						if (start != cur) {
							flag[start][Kpos][++(flag[start][Kpos][0])] = flag[cur][Kpos][now];
							flag[cur][Kpos][now] = -1; //remove it from flag[cur][Kpos]
						}
					}
				}
			}

			lastPosFlag[cur][Kpos] = now;

			if (Precorrect) {

				CurrentRating += NodeRating[flag[cur][Kpos][now - 1]];

				desire->push_back(flag[cur][Kpos][now - 1]);
				
				if (Continuous[Kpos] != Kpos - 2) {
					flag[Kpos - 2][Kpos][++flag[Kpos - 2][Kpos][0]] = flag[cur][Kpos][now - 1];
					flag[cur][Kpos][now - 1] = -1;
				}

			}
		}

		//test no visited node
		bool NoVisitedCorrect = false;

		if (!Precorrect) {
			
			int xpos = EndStartPos[Kpos];

			for (int end = (*invertList)[Kpos].size(); !NoVisitedCorrect && xpos < end; ++xpos) {

				NoVisitedCorrect = true;
					
				for (int i = 0; NoVisitedCorrect && i < endOfdesire; ++i) {
					if (SDist[((*invertList)[Kpos][xpos].mID)][(*desire)[i]] > curMaxPair) {
						NoVisitedCorrect = false;

						if (i) {
							flag[i][Kpos][++(flag[i][Kpos][0])] = ((*invertList)[Kpos][xpos].mID);
						}
					}
				}

			}

			EndStartPos[Kpos] = xpos;

			if (NoVisitedCorrect) { //in r-clique

				if (endOfdesire > 1) {
					flag[endOfdesire - 1][Kpos][++flag[endOfdesire - 1][Kpos][0]] = ((*invertList)[Kpos][xpos - 1].mID);
				}

				CurrentRating += NodeRating[((*invertList)[Kpos][xpos - 1].mID)];

				desire->push_back(((*invertList)[Kpos][xpos - 1].mID));

			}
		}

		//whether feasible keyword node set
		if ((Precorrect || NoVisitedCorrect)) {

			if (Kpos != endOfpos) {
				findOptimalSolution(Kpos + 1, endOfpos, curMaxPair); //find next keyword node

				if (Kpos == 1) 
					memset(EndStartPos + 2, 0, sizeof(int) * 9); //if it is the first keyword node then reset EndStartPos


				lastPosFlag[Kpos - 1][Kpos + 1] = 1;


				if (Kpos <= (endOfpos - 2)) {

					//reset continuous
					for (int loop = Kpos + 2; loop <= endOfpos; ++loop) {
						if (Continuous[loop] >= Kpos)
							Continuous[loop] = Kpos - 1;
					}

					//merge flag[Kpos][Kpos + 1...endOfpos] to flag[Kpos - 1][Kpos + 1...endOfpos]
					for (int loop = Kpos + 2; loop <= endOfpos; ++loop) {

						if (Kpos > 1 && flag[Kpos][loop][0]) {

							if (flag[Kpos - 1][loop][0]) {

								int temp[500] = {0};

								int pA = 1, pB = 1;
								int count = 1;

								for (; pA <= flag[Kpos][loop][0] && pB <= flag[Kpos - 1][loop][0];) {
									if (flag[Kpos - 1][loop][pB] != -1) {
										if (NodeRating[flag[Kpos][loop][pA]] > NodeRating[flag[Kpos - 1][loop][pB]]) {
											temp[count++] = flag[Kpos][loop][pA];
											++pA;
										} else {
											temp[count++] = flag[Kpos - 1][loop][pB];
											++pB;
										}	
									}
									else {
										++pB;
									}
								}

								if (pA <= flag[Kpos][loop][0]) {
									int remain = flag[Kpos][loop][0] - pA + 1;
									memcpy(temp + count, flag[Kpos][loop] + pA, remain * sizeof(int));
									count += remain;
								}

								if (pB <= flag[Kpos - 1][loop][0]) {
									for (int pos = pB; pos <= flag[Kpos - 1][loop][0]; ++pos) {
										if (flag[Kpos - 1][loop][pos] != -1)
											temp[count++] = flag[Kpos - 1][loop][pos];
									}
								}

								temp[0] = count - 1;

								memcpy(flag[Kpos - 1][loop], temp, count * sizeof(int));
							}
							else {
								memcpy(flag[Kpos - 1][loop], flag[Kpos][loop], (flag[Kpos][loop][0] + 1) * sizeof(int));
							}
						}

						//clear flag size
						flag[Kpos][loop][0] = 0;

						//clear flag visited
						lastPosFlag[Kpos][loop] = lastPosFlag[Kpos - 1][loop] = 1;
					}
				}

				CurrentRating -= NodeRating[(*desire)[Kpos - 1]];

				desire->pop_back();

			}
			else {
				if (CurrentRating > OptRating) {
					*result = *desire;
					OptRating = CurrentRating;
				}

				//topKResult->insert(*desire, CurrentRating);

				int now;

				for (now = lastPosFlag[cur][Kpos]; now <= candSize; ++now) {

					bool Test = true;

					for (int start = cur; Test && start < endOfdesire; ++start) {
						if (SDist[flag[cur][Kpos][now]][(*desire)[start]] > curMaxPair) {
							Test = false;

							//avoid repitition
							//move to flag[start][Kpos]
							if (start != cur) {
								flag[start][Kpos][++(flag[start][Kpos][0])] = flag[cur][Kpos][now];
								flag[cur][Kpos][now] = -1; //remove it from flag[cur][Kpos]
							}
						}
					}

					if (Test) {

						if (Continuous[Kpos] != Kpos - 2) {
							flag[Kpos - 2][Kpos][++flag[Kpos - 2][Kpos][0]] = flag[cur][Kpos][now];
							flag[cur][Kpos][now] = -1;
						}

					}
				}

				lastPosFlag[cur][Kpos] = now;

				Continuous[Kpos] = Kpos - 2;

				CurrentRating -= NodeRating[(*desire)[Kpos - 1]];

				desire->pop_back();

				return;

			}
		}
	}

	if (Kpos > 2)
		Continuous[Kpos] = Kpos - 2;
	
	return;
}

bool outputStatistic(int TotalTime) {
	FILE* StaFile = fopen("Result\\TestResult.txt", "a");

	if (NULL == StaFile)
		return false;

	fprintf(StaFile, "%d\n", TotalTime);

	fclose(StaFile);
	return true;
}