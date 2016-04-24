#include "SDist.h"

int SDist[MAX_NODE_NUM][MAX_NODE_NUM] = {0};

bool getAdjMatrix(std::string filename, int NodeNum) {
	FILE* AdjFile = fopen(filename.c_str(), "r");

	if (!AdjFile)
		return false;

	//get AdjMatrix
	bool input = true;

	int info[3]; //U, V, W

	while (input) {
		fscanf(AdjFile, "a %d %d %d\n", info, info + 1, info + 2);

		if (*info < NodeNum && *(info + 1) < NodeNum)
			SDist[info[0] - 1][info[1] - 1] = info[2];
		else
			input = false;
	}

	fclose(AdjFile);
	return true;
}

void Floyd(int NodeNum) {
	//SD[i][j] > SD[i][k] + SD[k][j] ?
	//Path[i][j] = Path[k][j] ?
	for (int k = 0; k < NodeNum; ++k) {
		for(int i = 0; i < NodeNum; ++i) {
			if (!SDist[i][k]) continue; //i to k unreachable
			for (int j = 0; j < NodeNum; ++j) {
				if (i == j) continue; // i to i
				if (!SDist[k][j]) continue; //k to j unreachable

				int temp = SDist[i][k] + SDist[k][j];

				if (!SDist[i][j] || temp < SDist[i][j]) {
					SDist[i][j] = temp;
				}
			}
		}
	}
}

bool outputSD(std::string filename, int NodeNum) {
	FILE* outAdjFile = fopen(filename.c_str(), "w");

	if (!outAdjFile)
		return false;

	for (int i = 0; i < NodeNum; ++i) {
		for (int j = 0; j < NodeNum - 1; ++j) {
			fprintf(outAdjFile, "%d ", SDist[i][j]);
		}
		fprintf(outAdjFile, "%d\n", SDist[i][NodeNum - 1]);
	}

	fclose(outAdjFile);
	return true;
}

bool getSDMatrix(std::string filename, int NodeNum) {
	FILE* SDFile = fopen(filename.c_str(), "r");

	if (!SDFile)
		return false;

	//get SDMatrix
	for (int i = 0; i < NodeNum; ++i) {
		for (int j = 0; j < NodeNum - 1; ++j) {
			fscanf(SDFile, "%d ", &SDist[i][j]);
		}
		fscanf(SDFile, "%d\n", &SDist[i][NodeNum - 1]);
	}

	fclose(SDFile);
	return true;
}

void generateSD() {
	char filename[4][20] = {"NYSD_1.sd", "NYSD_2.sd", "NYSD_3.sd", "NYSD_4.sd"};

	int NodeNum = 5000;

	getAdjMatrix("Data\\USA-road-d.NY.gr", MAX_NODE_NUM + 1);

	for (int i = 0; i < 4; ++i) {
		Floyd(NodeNum);
		outputSD(std::string(filename[i]), NodeNum);
		NodeNum += 5000;
	}
}