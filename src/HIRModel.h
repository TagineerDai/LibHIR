#pragma once
#include "HIRArgument.h"
#include "HIRConfig.h"
class HIRModel {
public:
	int ModelLoading(HIRConfig cfg);
	int ModelSaving(HIRConfig cfg);
	int GradInit(HIRConfig cfg);
	int GradClear(HIRConfig cfg);
	int GradForward(HIRConfig cfg, int* index_list, double* feature);
	int GradBackward(HIRConfig cfg, HIRTrainParam para, int* index_list);
	int ismat, dim;

	//should be made after the cfg and record have done and serve as the HIRModel class
	std::vector<double*> elist; // param per entity indentity [model.txt]
	std::vector<double*> mylist; // Linear M last-layer [model.txt]
	std::vector<double**> hlist; // param per hidden-layer [model.txt]
	std::vector<double***> Tnet; // Trans with tensor[model.txt]
	std::vector<double*> rlist; //∫ÕTnet+Hmatœ‡Õ¨

	//For backpropagation
	double loss; //
	double* dL_y; //	            [cfg.task]
	std::vector<double*> dy_my; //  [cfg.task][dim]
	std::vector<double*>dy_r; //    [cfg.task][dim]
	std::vector<double**> dr_e; //  [cfg.enum] x [dim][dim]
	std::vector<double**> dr_r; //  [size.size() + hnum - 1] x [dim][dim] 
	std::vector<double***> dr_T; // [size.size()] x [dim][dim][dim]
	std::vector<double**> dr_H; //  [cfg.hnum] x [dim][dim]

	std::vector<double*> dL_my; //  [cfg.task][dim]
	std::vector<double*>dL_r; //    [size.size() + hnum][dim]
	std::vector<double*> dL_e; //  [cfg.e_num] x [dim]
	std::vector<double***> dL_T; // [size.size()] x [dim][dim][dim]
	std::vector<double**> dL_H; //  [cfg.hnum] x [dim][dim]
	double* feature;
	std::vector<int> size, lidx, ridx;

	HIRModel();
	~HIRModel();
	HIRModel(HIRConfig cfg);
};
