#pragma once
#include<fstream>
#include<iostream>
#include "HIRArgument.h"
#include "HIRConfig.h"
class HIRRecord {
public:
	HIRRecord();
	~HIRRecord();
	HIRRecord(HIRConfig cfg, int istrain);
	void predictSave(std::string pfname, HIRConfig cfg);
	//int Evaluate(HIRConfig cfg, HIRModel model, int eva_mtd);
	//int PredictSaving(std::string filename);
	int rec, train;
	std::string filename;
	std::vector<int*> ilist; // index per record
	std::vector<double*> flist; // feature per record
	double** ylist; // target value for classification
	double** hat_y; // predict value of the model
};