#ifndef _TRAVERSEINFO_H
#define _TRAVERSEINFO_H

#include <vector>
#include "ExtraInfo.h" //for max keyword number

class TraverseInfo
{
public:
	TraverseInfo();
	TraverseInfo(int QueryKeywordNumber, std::vector<std::vector<int>> *invertList);
	~TraverseInfo();

	int Continuous[MAX_KEYWORD_NUM + 1];
	std::vector<int> *f[MAX_KEYWORD_NUM + 1][MAX_KEYWORD_NUM + 1];

private:
	int mKeywordNum;
};

#endif