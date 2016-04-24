#include "TraverseInfo.h"

TraverseInfo::TraverseInfo() {};

TraverseInfo::TraverseInfo(int QueryKeywordNumber):mKeywordNum(QueryKeywordNumber) {

	//construct f TraversInfo

	//f[0, 1]; f[0, 2], f[1, 2]; f[0, 3], f[1, 3], f[2, 3];...
	for (int i = 1; i <= mKeywordNum; ++i) {
		for (int j = 0; j < i; ++j) {
			f[j][i] = new std::vector<int>();
		}
	}

	//initial continuous array
	for (int i = 0; i < (MAX_KEYWORD_NUM + 1); ++i) {
		Continuous[i] = 0;
	}

}

TraverseInfo::~TraverseInfo() {

	//release f[][] array
	for (int i = 1; i <= mKeywordNum; ++i) {
		for (int j = 0; j < i; ++j) {
			delete f[j][i];
		}
	}
}