#include "HIRRecord.h"
HIRRecord::HIRRecord() {
	train = 1;
	rec = 0;
}

HIRRecord::~HIRRecord(){
	return;
}

HIRRecord::HIRRecord(HIRConfig cfg, int istrain) {
	train = istrain;
	if (train) {//train
		rec = cfg.rtrain;
		filename = cfg.tfname;
	}
	else {//test
		rec = cfg.rtest;
		filename = cfg.efname;
	}

	int i, j, k, r, layer;
	double *pnowd, tmpd;
	int *pnowi, tmpi;
	std::string line;
	std::ifstream ifs(filename);
	std::stringstream strs(""); strs.clear();

	// Open record file
	if (!ifs.is_open()) {
		std::cout << "Unable to open record file " << filename << std::endl;
		return;
	}

	//The tot number record
	std::getline(ifs, line);
	strs << line;
	strs >> tmpi;
	strs >> tmpi;
	if (rec != tmpi) rec = tmpi;
	//TEST
	/*	std::cout << train.filename << train.rec << std::endl; */

	ylist = new double*[rec];
	for (r = 0; r < rec; r++) {
		std::getline(ifs, line);
		strs.str("");
		strs.clear();
		strs << line;
		ylist[r] = new double[cfg.task];
		i = 0;
		while (strs >> tmpd && i<cfg.task) {
			ylist[r][i] = tmpd;
			i++;
		}
	}
	//TEST
	/*		for(int i = 0; i<test.rec; i++)
	for (int j = 0; j < cfg.task; j++)
	std::cout << i << "," << j << " = " << test.ylist[i][j] << std::endl;
	*/


	hat_y = new double*[rec];
	for (r = 0; r < rec; r++) {
		hat_y[r] = new double[cfg.task];
		for(i=0 ; i<cfg.task; i++)
			hat_y[r][i] = 0;
	}
	//TEST
	/*
	for(int i = 0; i<test.rec; i++)
	for (int j = 0; j < cfg.task; j++)
	std::cout << i << "," << j << " = " << test.hat_y[i][j] << std::endl;
	*/

	//The index of every record
	ilist.clear();
	for (r = 0; r < rec; r++) {
		std::getline(ifs, line);
		strs.str("");
		strs.clear();
		strs << line;
		pnowi = new int[cfg.e_num];
		i = 0;
		while (strs >> tmpi && i<cfg.e_num) {
			pnowi[i] = tmpi;
			i++;
		}
		ilist.push_back(pnowi);
	}
	//TEST
	/*	int r = 0;
	for (std::vector<int*>::iterator ip = train.ilist.begin();
		ip != train.ilist.end(); ip++, r++) {
		for (int i = 0; i < cfg.e_num; i++) {
			std::cout << r << "," << i << " = " << (*ip)[i] << std::endl;
		}
	}*/

	//The feature of every record
	flist.clear();
	for (r = 0; r < rec; r++) {
		std::getline(ifs, line);
		strs.str("");
		strs.clear();
		strs << line;
		pnowd = new double[cfg.fwid];
		i = 0;
		while (strs >> tmpd && i<cfg.fwid) {
			pnowd[i] = tmpd;
			i++;
		}
		flist.push_back(pnowd);
	}
	//TEST
	/*
	int r = 0;
	for (std::vector<double*>::iterator ip = train.flist.begin();
		ip != train.flist.end(); ip++, r++) {
		for (int i = 0; i < cfg.fwid; i++) {
			std::cout << r << "," << i << " = " << (*ip)[i] << std::endl;
		}
	}
	*/

	return ;
}

void HIRRecord::predictSave(std::string pfname, HIRConfig cfg) {
	
	// Open the predict.txt
	std::ofstream ofs(pfname);
	if (!ofs.is_open()) {
		std::cout << "Unable to open model file " << pfname << std::endl;
		return;
	}
	std::cout << "Predict answer is saved at " << pfname << std::endl;

	//record number
	ofs << rec << std::endl;
	
	//predicted value
	for (int r = 0; r < rec; r++) {
		for (int i = 0; i < cfg.task; i++)
			ofs << hat_y[r][i] << ' ';
		ofs << std::endl;
	}
	ofs.close();
}


/*
int HIRRecord::PredictSaving(std::string ftrain) {
	int r, i, tmpi;
	arecord rec;
	dftr pnow;
	double tmpd;
	std::string line;
	std::stringstream strs("");
	std::ifstream ifs(ftrain);
	// Open record.txt
	if (!ifs.is_open()) {
		std::cout << "Unable to open record file " << f << std::endl;
		return 1;
	}

	// The ylist after normalization 
	std::getline(ifs, line);
	strs.str(""); strs.clear();
	strs << line;
	while (strs >> tmpd) Train.ylist.push_back(tmpd);
	/* Test ylist done!
	i = 0;
	for (std::vector<double>::iterator iy = ylist.begin();
	iy != ylist.end(); iy++, i++) {
	std::cout << "y at " << i << " = " << *iy << std::endl;
	}

	// Index list of every record
	for (r = 0; r < cfg.record; r++) {
		std::getline(ifs, line);
		strs.str(""); strs.clear();
		strs << line;
		i = 0;
		rec = new int[cfg.e_num];//REMEBER TO DELETE!
		while (strs >> tmpi) {
			if (tmpi != 0)
				rec[i] = (idxe.at(i) - maxe.at(i)) + tmpi - 1;
			else
				rec[i] = -1;
			i++;
		}
		ilist.push_back(rec);
	}
	/*
	//The ilist is tested.
	//std::cout << idxe.at(i) - maxe.at(i) << " " << tmpi << " " << rec[i] << std::endl;
	r = 0;
	for (std::vector<arecord>::iterator ip = ilist.begin();
	ip != ilist.end(); ip++, r++) {
	for (i = 0; i < cfg.e_num; i++) {
	std::cout << r << "," << i << " = " << (*ip)[i] << std::endl;
	}
	}
	// Feature list of every record
	for (r = 0; r < cfg.record; r++) {
		std::getline(ifs, line);
		strs.str("");
		strs.clear();
		strs << line;
		pnow = new double[cfg.fwid]; //REMEMBER TO DELETE!
		i = 0;
		while (strs >> tmpd) {
			pnow[i] = tmpd;
			i++;
		}
		flist.push_back(pnow);
	}
	/* //The flist tested!
	r = 0;
	for (std::vector<dftr>::iterator ip = flist.begin();
	ip != flist.end(); ip++, r++) {
	for (i = 0; i < cfg.fwid; i++) {
	std::cout << r << "," << i << " = " << (*ip)[i] << std::endl;
	}
	}
	return 0;
}*/