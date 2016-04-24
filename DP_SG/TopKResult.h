#ifndef _TOPKRESULT_H
#define _TOPKRESULT_H

#include <vector>
#include "ExtraInfo.h" //for max keyword num

#define MAX_TOPK 10

typedef struct IndexRating {
	int Index;
	int Rating;
}IndexRating;

class TopKResult
{
public:
	TopKResult(int TopK, int KeywordNum);
	~TopKResult();

	void insert(std::vector<int> &result, int rating); //insert a result

	//get current minimum rating
	int getMinRating() { return IRArray[TopK - 1].Rating; } 

	void OutPutResultToFile(char filename[]);

private:
	int TopK;
	int curResultNum;
	IndexRating IRArray[MAX_TOPK + 1]; //the pos of result in TopK array and each result's rating
	int TopKArray[MAX_TOPK][MAX_KEYWORD_NUM]; //store topk result
	int curKeywordNum;
};



#endif