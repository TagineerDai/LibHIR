#include "HIR.h"

HIRRecord train;
HIRTrainParam para_train;

int forward_train(int, HIRConfig);
int backward_train(int, HIRConfig);


int main(int argc, char**argv) {

	//config init
	cfg = HIRConfig();

	//model init
	HIR = HIRModel(cfg);
	HIR.GradInit(cfg);

	//argument init	
	para_train = parse_train_argument(argc, argv, cfg.task);

	//std::cout << param.errmsg.size() << std::endl;
	if (para_train.errmsg.size() != 0) {
		printf("Error occured in parsering arguments.\n");
		return 0;
	}

	//record(train) init
	train = HIRRecord(cfg, 1);
	
	std::cout << "=== LOSS ===" << std::endl;
	
	std::vector<int> v;
	for (int i = 0; i < train.rec; ++i) {
		v.push_back(i);
	}
	int seed;
	for (int epoch = 0; epoch < 100; epoch++) {
		// forward
		// obtain a time-based seed:  
		seed = std::chrono::system_clock::now().time_since_epoch().count();
		std::shuffle(v.begin(), v.end(), std::default_random_engine(seed));

		for (int index = 0; index < train.rec; index++) {
			HIR.GradClear(cfg);
			forward_train(v.at(index), cfg);
			backward_train(v.at(index), cfg);
		}
		// evaluate
		double evl = evaluate(para_train.evl, train.rec, train.ylist, train.hat_y);
		std::cout << "epoch " << epoch <<" : " << evl << std::endl;
	}

	//HIR.ModelSaving(cfg);

	printf("HIR train ended.\n");
	printf("Press Enter to exit.");
	getchar();
	return 0;
}

int forward_train(int index, HIRConfig cfg) {
	int* eids = train.ilist.at(index);
	int x, y, z;
	int xx, yy, zz, lh, lr;
	double ztemp, ytemp, *vx, *vy, *vr, ***T, **M;
	xx = zz = cfg.dim;
	eids[0]; eids[1]; eids[2]; eids[3];

	//representation generating
	if (cfg.fwid == 0)
		HIR.GradForward(cfg, train.ilist.at(index), nullptr);
	else
		HIR.GradForward(cfg, train.ilist.at(index), train.flist.at(index));

	//M forward
	lr = cfg.hnum + cfg.size.size() - 1;
	vx = HIR.rlist.at(lr);
	yy = cfg.task;
	for (y = 0; y < yy; y++) {
		vy = HIR.mylist.at(y);
		train.hat_y[index][y] = dotMul(yy, vx, vy);
	}
	//TEST
	//double* hy = train.hat_y[0];
	//for (int i = 0; i<cfg.task; i++)
	//printf("%f\n", hy[i]);
	

	return 0;
}

int backward_train(int index, HIRConfig cfg) {
	int* eids = train.ilist.at(index);
	int x, y, z, i, j, k;
	int xx, yy, zz, lh, lr;
	double ztemp, ytemp, *vx, *vy, *vr, ***T, **M;
	//eids[0]; eids[1]; eids[2]; eids[3];


	//loss calc
	HIR.loss = loss(para_train.obj, train.ylist[index], train.hat_y[index]);
	//TEST
	/*std::cout << HIR.loss << std::endl;*/

	//d_y calc
	switch (para_train.obj) {
	case RMSE_L: {
		for (i = 0; i < cfg.task; i++)
			HIR.dL_y[i] = (train.hat_y[index][i] - train.ylist[index][i]) / HIR.loss;
		break;
	}
	case ABS_L: {
		for (i = 0; i < cfg.task; i++)
			HIR.dL_y[i] = (train.hat_y[index][i] > train.ylist[index][i]) ? (-1 / cfg.task) : (1 / cfg.task);
		break;
	}
	case LL_L: {
		for (i = 0; i < cfg.task; i++)
			HIR.dL_y[i] = (train.hat_y[index][i] - train.ylist[index][i]) / cfg.task;
		break;
	}
	case GOLD_L: {
		for (i = 0; i < cfg.task; i++)
			HIR.dL_y[i] = (train.hat_y[index][i] - train.ylist[index][i])/cfg.task;
		break;
	}
	}

	//M backward [TESTED]
	
	lr = cfg.hnum + cfg.size.size() - 1;
	for (i = 0; i < cfg.task; i++) {
		for (j = 0; j < cfg.dim; j++) {
			HIR.dy_r.at(i)[j] = HIR.mylist.at(i)[j];
			HIR.dL_r.at(lr)[j] += HIR.dL_y[i] * HIR.dy_r.at(i)[j];

			HIR.dy_my.at(i)[j] = HIR.rlist.at(lr)[j];
			HIR.dL_my.at(i)[j] = HIR.dL_y[i] * HIR.dy_my.at(i)[j];
		}
	}
	//M update [TESTED]
	for (i = 0; i < cfg.task; i++)
		for (j = 0; j < cfg.dim; j++)
			HIR.mylist.at(i)[j] += -1 * para_train.alpha  * HIR.dL_my.at(i)[j];
	for (i = 0; i < cfg.dim; i++)
			HIR.rlist.at(lr)[i] += -1 * para_train.alpha * HIR.dL_r.at(lr)[i];
	//feature copy
	if(cfg.fwid)
		for (int i = 0; i < cfg.fwid; i++) {
			HIR.feature[i] = train.flist.at(index)[i];
		}

	//model learning
	HIR.GradBackward(cfg, para_train, train.ilist.at(index));

	return 0;
}