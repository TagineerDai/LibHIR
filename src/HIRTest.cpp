#include "HIR.h"

HIRRecord test;
HIRTestParam para_test;

int forward_test(int, HIRConfig);

int main(int argc, char**argv) {

	//config init
	cfg = HIRConfig();

	//model init
	HIR = HIRModel(cfg);
	HIR.GradInit(cfg);
	
	//argument init	
	para_test = parse_test_argument(argc, argv, cfg.task);
	
	//std::cout << param.errmsg.size() << std::endl;
	if (para_test.errmsg.size() != 0) {
		printf("Error occured in parsering arguments.\n");
		return 0;
	}

	//record(train) init
	test = HIRRecord(cfg, 0);

	// forward
	for (int index = 0; index < test.rec; index++) {
		HIR.GradClear(cfg);
		forward_test(index, cfg);
	}

	// evaluate
	double evl = evaluate(para_test.evl, test.rec, test.ylist, test.hat_y);
	std::cout << "The evaluation result is: " << evl << std::endl;

	test.predictSave(para_test.pfname, cfg);

	printf("HIR test ended.\n");
	printf("Press Enter to exit.");
	getchar();
	return 0;
}

int forward_test(int index, HIRConfig cfg) {
	int* eids = test.ilist.at(index);
	int x, y, z;
	int xx, yy, zz, lh, lr;
	double ztemp, ytemp, *vx, *vy, *vr, ***T, **M;
	xx = zz = cfg.dim;
	eids[0]; eids[1]; eids[2]; eids[3];
	
	//representation generating
	if (cfg.fwid == 0)
		HIR.GradForward(cfg, test.ilist.at(index), nullptr);
	else
		HIR.GradForward(cfg, test.ilist.at(index), test.flist.at(index));

	//M forward
	lr = cfg.hnum + cfg.size.size() - 1;
	vx = HIR.rlist.at(lr);
	yy = cfg.task;
	for (y = 0; y < yy; y++) {
		vy = HIR.mylist.at(y);
		test.hat_y[index][y] = dotMul(yy, vx, vy);
	}
	//TEST
	//double* hy = test.hat_y[0];
	//for (int i = 0; i<cfg.task; i++)
	//printf("%f\n", hy[i]);
	

	return 0;
}
