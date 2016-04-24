#include <stdio.h>
#include <string.h>
#include "Graph.h"

int **AdjMatrix;

bool getPraticalGraph(char filename[], int NodeNum) {
	FILE* AdjFile = fopen(filename, "r");

	if (!AdjFile)
		return false;

	//get AdjMatrix
	bool input = true;

	int info[3]; //U, V, W

	while (input) {
		fscanf(AdjFile, "a %d %d %d\n", info, info + 1, info + 2);

		if (*info < NodeNum && *(info + 1) < NodeNum)
			AdjMatrix[info[0] - 1][info[1] - 1] = info[2];
		else
			input = false;
	}

	fclose(AdjFile);
	return true;
}

bool getAdjMatrix(char filename[], int NodeNum) {
	FILE* AdjFile = fopen(filename, "r");

	if (!AdjFile)
		return false;

	for (int i = 0; i < NodeNum; ++i) {
		for (int j = 0; j < NodeNum; ++j) {
			fscanf(AdjFile, "%d", &AdjMatrix[i][j]);
		}
	}

	fclose(AdjFile);
	return true;
}

void ConstructGlobalMatrix(int NodeNum) {
	
	AdjMatrix = new int *[NodeNum];

	for (int i = 0; i < NodeNum; ++i) {
		AdjMatrix[i] = new int [NodeNum];
		memset(AdjMatrix[i], 0, sizeof(int)*NodeNum);
	}
}

void ReleaseGlobalMatrix(int NodeNum) {

	for (int i = 0; i < NodeNum; ++i) {
		delete [] AdjMatrix[i];
	}

	delete [] AdjMatrix;
}