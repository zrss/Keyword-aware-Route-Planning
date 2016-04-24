#include "TopKResult.h"

TopKResult::TopKResult(int K, int KeywordNum):TopK(K), curResultNum(0), curKeywordNum(KeywordNum) {
	memset(IRArray, 0, sizeof(IRArray));
}

TopKResult::~TopKResult() {}

void TopKResult::insert(std::vector<int> &result, int rating) {
	
	//find a feasible position to insert
	int pos = 0; bool stop = false;

	for (; !stop && pos < curResultNum; ++pos) {
		if (rating > IRArray[pos].Rating) {
			stop = true;
		}
	}

	if (stop)
		--pos;

	if (pos != TopK) { //catch the pos

		//moveback index
		for (int rpos = curResultNum - 1; rpos >= pos; --rpos) {
			IRArray[rpos + 1] = IRArray[rpos];
		}

		int InsertIndex;

		if (TopK == curResultNum) //swap last one
			InsertIndex = IRArray[curResultNum].Index;
		else { //insert it
			InsertIndex = curResultNum;
			++curResultNum;
		}

		int count = 0;

		//store 
		for (std::vector<int>::iterator itr = result.begin(); itr != result.end(); ++itr) {
			TopKArray[InsertIndex][count++] = *itr;
		}

		IRArray[pos].Index = InsertIndex;
		IRArray[pos].Rating = rating;

	}

}

void TopKResult::OutPutResultToFile(char filename[]) {
	FILE* file = fopen(filename, "a");

	if (file == NULL)
		return;

	/*for (int answer = 0; answer < curResultNum; ++answer) {
		
		int rank = IRArray[answer].Index;
		
		for (int count = 0; count < curKeywordNum; ++count) {
			fprintf(file, "%d ", TopKArray[rank][count]);
		}

		fprintf(file, "\n");
	}

	if (!curResultNum)
		fprintf(file, "\n");*/

	for (int count = 0; count < TopK; ++count) {
		fprintf(file, "%d\n", IRArray[count].Rating);
	}

	fclose(file);
}