#pragma once
#include<vector>
#ifdef WIN32 //windows
#include <cstring>
#else //linux
#include <string>
#endif
#define INF 1.79e+308 //double

enum {
	ABS_L, RMSE_L, LL_L, GOLD_L,
	ABS_E, RMSE_E, AUC_E, ACC_E,
};
enum mission{
	TRAIN, TEST
};
struct HIRTrainParam{
	int epoch;// integer >= 1
	int obj; //objection function
	int evl; //evaluation method
	int learn_mtd; //default SGD 0
	double alpha; //the learning rate of SGD
	std::vector<std::string> errmsg;
};

struct HIRTestParam {
	int evl; //evaluation method
	std::string pfname; //the predict output filename
	std::vector<std::string>errmsg;
};

void exit_with_help(int mission);

HIRTrainParam parse_train_argument(int argc, char **argv, int task);

HIRTestParam parse_test_argument(int argc, char **argv, int task);
