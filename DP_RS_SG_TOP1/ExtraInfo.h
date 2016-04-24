#ifndef _KEYWORD_H
#define _KEYWORD_H

#include <string>
#include "SDist.h"

#define MAX_KEYWORD_NUM 10 

#define MAX_RATING 10 //rating score <= MAX_RATING

extern int NodeKW[MAX_NODE_NUM]; //node keyword info
extern int NodeRating[MAX_NODE_NUM]; //node rating score

typedef enum KeyWord {
	Non = 0,
	Shop = 1,
	Bar = 2,
	Hospital = 4,
	Library = 8,
	Restaurant = 16,
	Hotel = 32,
	Park = 64,
	Cinema = 128,
	Gym = 256,
	Bank = 512
}KeyWord;

//generate keyword from Non to MaxKeywordNumber [0, 1, 2, 4, ..., 2^MaxKeyWordNum]
//then output to file filename
bool generateKeyword(int NodeNum, int MaxKeyWordNum, std::string filename);

//random generate rating score for NodeNum
//rating score range from [0...MaxRating - 1]
bool generateRating(int NodeNum, int MaxRating, std::string filename);

//read the data from file and store them in Store array
bool getData(std::string filename, int NodeNum, int *Store);

#endif

