#include "BlockPool.h" //for BlockPool
#include "ExtraInfo.h" //for NodeKW and NodeRating
#include "CoSKQ.h"

/*

int main() {

	BlockPool blockPool;
	blockPool.ReadBlockIDtoMem("BlockIndex\\BlockID", MAX_NODE_NUM);

	getData("Data\\Keyword.kw", 20000, NodeKW);
	getData("Data\\LRating.ra", 20000, NodeRating);

	int QueryNode = 0;
	int MaxDist = 0, MaxPairDist = 0;
	std::vector<KeyWord> Look_For;

	while (true) {

		printf("input CoSKQ query\n");

		scanf("%d", &QueryNode);

		if (QueryNode == -1)
			break;

		scanf("%d %d", &MaxDist, &MaxPairDist);

		int keyword;
		Look_For.clear();

		bool correct = true;
		while (true) {
			scanf("%d", &keyword);

			if (keyword) {
				Look_For.push_back((KeyWord)keyword);
			} 
			else
				break;
		}

		//for (int Node = QueryNode; Node < MAX_NODE_NUM; Node += 1) {
			Rating_CoSKQ_Query(QueryNode, Look_For, std::pair<int, int>(MaxDist, MaxPairDist), blockPool);
			//printf("QueryNode: %d\n", Node);
		//}

		printf("finished\n");
		
	}

	return 0;
}*/


int main(void) {
	int MaxDist = 0, MaxPairDist = 0;
	std::vector<KeyWord> Look_For;
	int keyword;

	int choice = 0;
	bool running = true;
	std::pair<int, int> Range;

	BlockPool blockPool;
	blockPool.ReadBlockIDtoMem("BlockIndex\\BlockID", MAX_NODE_NUM);

	getData("Data\\Keyword.kw", MAX_NODE_NUM, NodeKW);
	getData("Data\\LRating.ra", MAX_NODE_NUM, NodeRating);

	while (running) {
		printf("1: Test Max Distance\n2: Test Max Pairwise Distance\n3: Test Keyword Number\n0: exit\n");
		scanf("%d", &choice);

		if (choice) {
			printf("input lowerID and upperID [lowerID, upperID)\n");
			scanf("%d %d", &Range.first, &Range.second);

			if (choice != 3) {
				printf("input Keyword end with 0\n");
				Look_For.clear();

				while (true) {
					scanf("%d", &keyword);

					if ((KeyWord)keyword) {
						Look_For.push_back((KeyWord)keyword);
					} 
					else
						break;
				}
			}

			switch (choice) {
			case 1:
				printf("input Max Pairwise Distance\n");
				scanf("%d", &MaxPairDist);

				printf("Start CoSKQ query\n\n");

				for (int QueryNode = Range.first; QueryNode < Range.second; QueryNode += 20) {
					for (int MaxDist = 5000; MaxDist < 35000; MaxDist += 5000) {
						Rating_CoSKQ_Query(QueryNode, Look_For, std::pair<int, int>(MaxDist, MaxPairDist), blockPool);
					}

					FILE* StaFile = fopen("Result\\TestResult.txt", "a");
					fprintf(StaFile, "\n");
					fclose(StaFile);
				}
					
				break;

			case 2:
				printf("input Max Distance\n");
				scanf("%d", &MaxDist);

				printf("Start CoSKQ query\n\n");

				for (int QueryNode = Range.first; QueryNode < Range.second; QueryNode += 20) {
					for (int MaxPairDist = 1000; MaxPairDist < 6000; MaxPairDist += 1000) {
						Rating_CoSKQ_Query(QueryNode, Look_For, std::pair<int, int>(MaxDist, MaxPairDist), blockPool);
					}

					FILE* StaFile = fopen("Result\\TestResult.txt", "a");
					fprintf(StaFile, "\n");
					fclose(StaFile);
				}

				break;

			case 3:
				printf("input Max Distance\n");
				scanf("%d", &MaxDist);

				printf("input Max Pairwise Distance\n");
				scanf("%d", &MaxPairDist);

				printf("Start CoSKQ query\n\n");

				KeyWord allKeyword[] = {Shop, Bar, Hospital, Library, Restaurant, Hotel, Park, Cinema, Gym, Bank};

				for (int QueryNode = Range.first; QueryNode < Range.second; QueryNode += 20) {
					for (int KeywordNum = 2; KeywordNum < 12; KeywordNum += 2) {
						std::vector<KeyWord> KSet(allKeyword, allKeyword + KeywordNum);
						Rating_CoSKQ_Query(QueryNode, KSet, std::pair<int, int>(MaxDist, MaxPairDist), blockPool);
					}

					FILE* StaFile = fopen("Result\\TestResult.txt", "a");
					fprintf(StaFile, "\n");
					fclose(StaFile);
				}

				break;
			}
		}
		else
			running = false;
	}

	return 0;
}