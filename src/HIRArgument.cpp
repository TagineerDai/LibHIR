#include "HIRArgument.h"
#include<cstdlib>
#include<iostream>

/*Print help message to std::cerr and exit.*/
void exit_with_help(int mission) {
	if (mission == TRAIN)
		std::cerr <<
		"Usage : HIRTrain [options]\n" <<
		"Sample : \n" <<
		"  HIRTrain -m 0 -r 100 -o 0-e 1 -a 0.05\n" <<
		"options : \n" <<
		"-r epoch : integer (default 1)\n" <<
		"-o objective function" <<
		"  for regression" <<
		"    0 -- abstract error loss funciton\n" <<
		"    1 -- root-mean-squrare error loss function\n" <<
		"  for (multi-class) classification\n" <<
		"    2 -- log-loss function\n" <<
		"    3 -- zero-one loss function\n" <<
		"-e evaluation method\n" <<
		"  for regression\n" <<
		"    4 -- abstract error evaluation\n" <<
		"    5 -- root-mean-squrare error evaluation\n" <<
		"  for (multi-class) classification\n" <<
		"    6 -- area under roc curve evaluation\n" <<
		"    7 -- accuracy evaluation\n" <<
		"-a alpha : double (0,1] (default 0.1)\n" <<
		"-m learning method : (default SGD)\n" <<
		"    0 -- Stochastic Gradient Desent\n" <<
		"-h help\n" <<
		"Press Enter to exit.\n";
	else  //(mission == TEST)
		std::cerr <<
		"Usage : HIRTest [options]\n" <<
		"Sample : \n" <<
		"  HIRTest -e 1\n" <<
		"options : \n" << 
		"-e evaluation method\n" <<
		"  for regression\n" <<
		"    4 -- abstract error evaluation\n" <<
		"    5 -- root-mean-squrare error evaluation\n" <<
		"  for (multi-class) classification\n" <<
		"    6 -- area under roc curve evaluation\n" <<
		"    7 -- accuracy evaluation\n" <<
		"-f predict answer filename\n"<<
		"-h help\n" <<
		"Press Enter to exit.\n";
	getchar();
	exit(0);
}

HIRTrainParam parse_train_argument(int argc, char **argv, int task)
{
	HIRTrainParam param;
	// init with a normal value
	param.errmsg.clear();
	param.learn_mtd = 0;
	param.epoch = 10;
	if (task == 1) {//Regression
		param.obj = RMSE_L;
		param.evl = RMSE_E;
	}
	else {//Classification
		param.obj = GOLD_L;
		param.evl = ACC_E;
	}
	param.alpha = 0.1;

	//parse the argument
	int epoch, evl, obj, mtd, i;
	double alpha;
	// parse options
	for (i = 1; i<argc; i++)
	{
		if (argv[i][0] != '-') break;
		if (++i >= argc)
			exit_with_help(TRAIN);
		switch (argv[i - 1][1])
		{
		case 'r':
			epoch = atoi(argv[i]);
			if (epoch >= 1)
				param.epoch = epoch;
			else(param.errmsg.push_back("Argument Parser--The parameter epoch invalid."));
			break;
		case 'e':
			evl = atoi(argv[i]);
			if ((task == 1 && (evl == ABS_E || evl == RMSE_E)) ||
				(task > 1 && (evl == AUC_E || evl == ACC_E)))
				param.evl = evl;
			else(param.errmsg.push_back("Argument Parser--The parameter evaluation method invalid."));
			break;
		case 'o':
			obj = atoi(argv[i]);
			if ((task == 1 && (obj == ABS_L || obj == RMSE_L)) ||
				(task != 1 && (obj == LL_L || obj == GOLD_L)))
				param.obj = obj;
			else(param.errmsg.push_back("Argument Parser--The parameter object function invalid."));
			break;
		case 'a':
			alpha = atof(argv[i]);
			if (alpha > 0 && alpha <= 1)
				param.alpha = alpha;
			else(param.errmsg.push_back("Argument Parser--The parameter alpha invalid."));
			break;
		case 'm':
			mtd = atoi(argv[i]);
			if (mtd == 0)
				param.learn_mtd = mtd;
			else(param.errmsg.push_back("Argument Parser--The parameter learn_method invalid."));
			break;
		case 'h':
			exit_with_help(TRAIN);
			break;
		default:
			std::string msg = "Argument Parser--Unknown option: -" + argv[i - 1][1];
			param.errmsg.push_back(msg);
			break;
		}
	}

	/*
	for (int i = 0; i < argc; i++) {
		std::cout << argv[i] << std::endl;
	}*/
	//DEBUG
	
	/*
	if (param.errmsg.size() > 0) {
		for (std::vector<std::string>::iterator msg = param.errmsg.begin(); 
			msg != param.errmsg.end(); 
			msg++) {
			std::cerr << msg->c_str() << std::endl;
			//There are something wrong
		}
		exit_with_help(TRAIN);
	}*/ 
	//OUT ERR MSG 

	return param;
}

HIRTestParam parse_test_argument(int argc, char **argv, int task) {
	HIRTestParam param;
	int i;
	param.errmsg.clear();
	if (task == 1) {//Regression
		param.evl = RMSE_E;
	}
	else {//Classification
		param.evl = ACC_E;
	}
	param.pfname = "predict.txt";
	int evl;
	// parse options
	for (i = 1; i<argc; i++)
	{
		if (argv[i][0] != '-') break;
		if (++i >= argc)
			exit_with_help(TEST);
		switch (argv[i - 1][1])
		{
		case 'e':
			evl = atoi(argv[i]);
			if ((task == 1 && (evl == ABS_E || evl == RMSE_E)) ||
				(task != 1 && (evl == AUC_E || evl == ACC_E)))
				param.evl = evl;
			else(param.errmsg.push_back("Argument Parser--The parameter evaluation method invalid."));
			break;
		case 'f':
			param.pfname = std::string(argv[i]);
			break;
		case 'h':
			exit_with_help(TEST);
			break;
		default:
			std::string msg = "Argument Parser--Unknown option: -" + argv[i - 1][1];
			param.errmsg.push_back(msg);
			break;
		}
	}
	return param;
}