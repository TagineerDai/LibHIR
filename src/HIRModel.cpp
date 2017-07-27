#include <fstream>
#include<iostream>
#include<sstream>
#include"HIRModel.h"

HIRModel::HIRModel() {
	dim = 0;
	lidx.clear();
	ridx.clear();
	size.clear();
	ismat = 0;
	return;
}

HIRModel::HIRModel(HIRConfig cfg) {
	//build the net with cfg.ismat and cfg.size
	ismat = cfg.ismat;
	dim = cfg.dim;
	//Cpy for discon
	for (std::vector<int>::iterator ip = cfg.maxe.begin(); ip != cfg.maxe.end(); ip++) {
		lidx.push_back((*ip));
	}
	for (std::vector<int>::iterator ip = cfg.idxe.begin(); ip != cfg.idxe.end(); ip++) {
		ridx.push_back((*ip));
	}
	for (std::vector<int>::iterator il = cfg.size.begin(); il != cfg.size.end(); il++) {
		size.push_back((*il));
	}

	ModelLoading(cfg);
}

HIRModel::~HIRModel() {
}

int HIRModel::ModelLoading(HIRConfig cfg) {
	double* pnow, tmpd;
	int e, h, i, j, k;
	int xx, yy, zz;
	double*** transT;
	double **mnow;
	std::string line, filename = cfg.mfname;
	std::ifstream ifs(filename);
	std::stringstream strs(""); strs.clear();

	// Open the model.txt
	if (!ifs.is_open()) {
		std::cout << "Unable to open model file " << filename << std::endl;
		return 1;
	}

	//The feature of every entity item
	int item;
	item = cfg.idxe.at(cfg.e_num - 1) + 1;
	elist.clear();
	for (e = 0; e < item; e++) {
		std::getline(ifs, line);
		strs.str("");
		strs.clear();
		strs << line;
		pnow = new double[cfg.dim]; //REMEMBER TO DELETE!
		i = 0;
		while (strs >> tmpd && i<cfg.dim) {
			pnow[i] = tmpd;
			i++;
		}
		elist.push_back(pnow);
	}
	/* Test the elist, done!!!!!!
	r = 0;
	for (std::vector<double*>::iterator ip = HIR.elist.begin();
	ip != HIR.elist.end(); ip++, r++) {
	for (i = 0; i < cfg.dim; i++) {
	std::cout << r << "," << i << " = " << (*ip)[i] << std::endl;
	}
	}*/

	// The parameter of the last layer
	mylist.clear();
	for (e = 0; e < cfg.task; e++) {
		std::getline(ifs, line);
		strs.str(""); strs.clear();
		strs << line;
		pnow = new double[cfg.dim]; //REMEMBER TO DELETE!
		i = 0;
		while (strs >> tmpd && i<cfg.dim) {
			pnow[i] = tmpd;
			i++;
		}
		mylist.push_back(pnow);
	}

	// Test Mylist Linear. DONE
	/*	for (std::vector<double*>::iterator ip = HIR.mylist.begin();
		ip != HIR.mylist.end(); ip++) {
		for (int i = 0; i < cfg.dim; i++) {
			std::cout << (*ip)[i] << ' ' << std::endl;
		}
	}*/


	// The parameter of every hidden layer
	hlist.clear();
	for (h = 0; h < cfg.hnum; h++) {
		mnow = new double*[cfg.dim];
		for (i = 0; i < cfg.dim; i++) {
			mnow[i] = new double[cfg.dim];
			std::getline(ifs, line);
			strs.str("");
			strs.clear();
			strs << line;
			j = 0;
			while (strs >> tmpd && j < cfg.dim) {
				mnow[i][j] = tmpd;
				j++;
			}
		}
		hlist.push_back(mnow);
	}
	/* Test the hlist, done!!!!!!
	h = 0;
	for (std::vector<double**>::iterator ip = HIR.hlist.begin();
	ip != HIR.hlist.end(); ip++, h++) {
	for (i = 0; i < cfg.dim; i++) {
	for (j = 0; j < cfg.dim; j++) {
	std::cout << h << "," << i << " = " << (*ip)[i][j] << std::endl;
	}
	}
	}*/

	// The transform network
	Tnet.clear();
	for (std::vector<int>::iterator iy = size.begin(); iy != size.end(); iy++){
		zz = cfg.dim;
		yy = *iy;
		xx = cfg.dim;
		
		transT = new double**[zz];
		for (i = 0; i < zz; i++) {
			transT[i] = new double*[yy];
			for (j = 0; j < yy; j++) {
				transT[i][j] = new double[xx];
				std::getline(ifs, line);
				strs.str("");
				strs.clear();
				strs << line;
				k = 0;
				while (strs >> tmpd && k < xx) {
					transT[i][j][k] = tmpd;
					k++;
				}
			}
		}
		Tnet.push_back(transT);
	}
	/*
	// Test Tensor
	int layer = 0;
	for (std::vector<double***>::iterator ip = HIR.Tnet.begin();
		ip != HIR.Tnet.end(); ip++, layer++) {
		for (int i = 0; i < cfg.dim; i++) {
			for (int j = 0; j < HIR.size.at(layer); j++) {
				std::cout << layer << " " << i << " === ";
				for (int k = 0; k < cfg.dim; k++){
						std::cout << (*ip)[i][j][k] << ' ' << std::endl;
				}
			}
		}
	}*/

	feature = new double[cfg.fwid];

	return 0;

}

int HIRModel::ModelSaving(HIRConfig cfg) {
	int i, j, k, layer;
	std::ofstream ofs(cfg.mfname);

	// Open the model.txt
	if (!ofs.is_open()) {
		std::cout << "Unable to open model file " << cfg.mfname << std::endl;
		return 1;
	}
	std::cout << "Save at " << cfg.mfname << std::endl;

	//The feature of every entity item
	for (std::vector<double*>::iterator ip = elist.begin();
		ip != elist.end(); ip++) {
		for (i = 0; i < cfg.dim; i++)
			ofs << (*ip)[i] << ' ';
		ofs.put('\n');
	}
	// the last layer
	for (std::vector<double*>::iterator ip = mylist.begin();
		ip != mylist.end(); ip++) {
		for (i = 0; i < cfg.dim; i++)
			ofs << (*ip)[i] << ' ';
		ofs.put('\n');
	}
	// The parameter of every hidden layer
	for (std::vector<double**>::iterator ip = hlist.begin();
		ip != hlist.end(); ip++) {
		for (i = 0; i < cfg.dim; i++) {
			for (j = 0; j < cfg.dim; j++) {
				ofs << (*ip)[i][j] << " ";
			}
			ofs.put('\n');
		}
	}
	layer = 0;
	// The transform network
		// Using Tensor
	for (std::vector<double***>::iterator ip = Tnet.begin();
		ip != Tnet.end(); ip++, layer++) {
		for (i = 0; i < cfg.dim; i++)
			for (j = 0; j < size.at(layer); j++) {
				for (k = 0; k < cfg.dim; k++)
					ofs << (*ip)[i][j][k] << " ";
				ofs.put('\n');
			}
	}
	return 0;
}

int HIRModel::GradInit(HIRConfig cfg) {
	double* pnow, **mnow, *** transT;
	int h, i, j, k;
	int xx, yy, zz;

	//double loss;
	loss = 0;

	//double* dL_y; //[cfg.task]
	dL_y = new double[cfg.task];
	for (i = 0; i < cfg.task; i++)
		dL_y[i] = 0;

	//std::vector<double*> dy_my; //  [cfg.task][dim]
	dy_my.clear();
	for (i = 0; i < cfg.task; i++) {
		pnow = new double[cfg.dim];
		for (j = 0; j < cfg.dim; j++)
			pnow[j] = 0;
		dy_my.push_back(pnow);
	}

	//std::vector<double*> dL_my; //  [cfg.task][dim]
	dL_my.clear();
	for (i = 0; i < cfg.task; i++) {
		pnow = new double[cfg.dim];
		for (j = 0; j < cfg.dim; j++)
			pnow[j] = 0;
		dL_my.push_back(pnow);
	}
	
	//std::vector<double*> rlist;
	rlist.clear();
	int rnum = cfg.size.size() + cfg.hnum;
	for (i = 0; i < rnum; i++) {
		pnow = new double[cfg.dim];
		for (j = 0; j < cfg.dim; j++)
			pnow[j] = 0;
		rlist.push_back(pnow);
	}

	//std::vector<double*>dy_r; //    [cfg.task][dim]
	dy_r.clear();
	for (i = 0; i < cfg.task; i++) {
		pnow = new double[cfg.dim];
		for (j = 0; j < cfg.dim; j++)
			pnow[j] = 0;
		dy_r.push_back(pnow);
	}



	//std::vector<double**> dr_r; //  [size.size() + hnum - 1] x [dim][dim] 
	dr_r.clear();
	int lr = cfg.size.size() + cfg.hnum - 1;
	for (i = 0; i < lr; i++) {
		mnow = new double*[cfg.dim];
		for (j = 0; j < cfg.dim; j++) {
			mnow[j] = new double[cfg.dim];
			for (k = 0; k < cfg.dim; k++)
				mnow[j][k] = 0;
		}
		dr_r.push_back(mnow);
	}

	//std::vector<double*>dL_r; //    [size.size() + hnum][dim]
	dL_r.clear();
	lr = cfg.size.size() + cfg.hnum;
	for (i = 0; i < lr; i++) {
		pnow = new double[cfg.dim];
		for (j = 0; j < cfg.dim; j++)
			pnow[j] = 0;
		dL_r.push_back(pnow);
	}

	//std::vector<double**> dr_e; //  [cfg.enum] x [dim][dim]
	dr_e.clear();
	for (i = 0; i < cfg.e_num; i++) {
		mnow = new double*[cfg.dim];
		for (j = 0; j < cfg.dim; j++) {
			mnow[j] = new double[cfg.dim];
			for (k = 0; k < cfg.dim; k++)
				mnow[j][k] = 0;
		}
		dr_e.push_back(mnow);
	}

	//std::vector<double**> dL_e; //  [cfg.e_num] x [dim]
	dL_e.clear();
	for (i = 0; i < cfg.e_num; i++) {
		pnow = new double[cfg.dim];
		for (j = 0; j < cfg.dim; j++)
			pnow[j] = 0;
		dL_e.push_back(pnow);
	}

	//std::vector<double**> dr_H; //  [cfg.hnum] x [dim][dim]
	dr_H.clear();
	for (h = 0; h < cfg.hnum; h++) {
		mnow = new double*[cfg.dim];
		for (i = 0; i < cfg.dim; i++) {
			mnow[i] = new double[cfg.dim];
			for (j = 0; j < cfg.dim; j++)
				mnow[i][j] = 0;
		}
		dr_H.push_back(mnow);
	}

	//std::vector<double**> dL_H; //  [cfg.hnum] x [dim][dim]
	dL_H.clear();
	for (h = 0; h < cfg.hnum; h++) {
		mnow = new double*[cfg.dim];
		for (i = 0; i < cfg.dim; i++) {
			mnow[i] = new double[cfg.dim];
			for (j = 0; j < cfg.dim; j++)
				mnow[i][j] = 0;
		}
		dL_H.push_back(mnow);
	}


	//std::vector<double***> dr_T; // [size.size()] x [dim][size(i)][dim]
	dr_T.clear();
	for (std::vector<int>::iterator iy = size.begin(); iy != size.end(); iy++) {
		zz = cfg.dim;
		yy = *iy;
		xx = cfg.dim;

		transT = new double**[zz];
		for (i = 0; i < zz; i++) {
			transT[i] = new double*[yy];
			for (j = 0; j < yy; j++) {
				transT[i][j] = new double[xx];
				for (k = 0; k < xx; k++)
					transT[i][j][k] = 0;
			}

		}
		dr_T.push_back(transT);
	}

	//std::vector<double***> dL_T; // [size.size()] x [dim][dim][dim]
	dL_T.clear();
	for (std::vector<int>::iterator iy = size.begin(); iy != size.end(); iy++) {
		zz = cfg.dim;
		yy = *iy;
		xx = cfg.dim;

		transT = new double**[zz];
		for (i = 0; i < zz; i++) {
			transT[i] = new double*[yy];
			for (j = 0; j < yy; j++) {
				transT[i][j] = new double[xx];
				for (k = 0; k < xx; k++)
					transT[i][j][k] = 0;
			}

		}
		dL_T.push_back(transT);
	}
	return 0;
}

int HIRModel::GradClear(HIRConfig cfg) {
	int i, j, k;

	//double loss;
	loss = 0;
	
	//double* dL_y; //                 [cfg.task]
	for (i = 0; i < cfg.task; i++)
		dL_y[i] = 0;

	//std::vector<double*> dy_my; //  [cfg.task][dim]
	for(std::vector<double*>::iterator ip = dy_my.begin(); ip!=dy_my.end(); ip++)
		for (j = 0; j < cfg.dim; j++)
			(*ip)[j] = 0;

	//std::vector<double*> dL_my; //  [cfg.task][dim]
	for (std::vector<double*>::iterator ip = dL_my.begin(); ip != dL_my.end(); ip++)
		for (j = 0; j < cfg.dim; j++)
			(*ip)[j] = 0;
	
	//std::vector<double*>dy_r; //    [cfg.task][dim]
	for (std::vector<double*>::iterator ip = dy_r.begin(); ip != dy_r.end(); ip++)
		for (j = 0; j < cfg.dim; j++)
			(*ip)[j] = 0;

	//std::vector<double**> dr_r; //  [size.size() + hnum - 1] x [dim][dim] 
	for (std::vector<double**>::iterator ip = dr_r.begin(); ip != dr_r.end(); ip++)
		for (i = 0; i < cfg.dim; i++)
			for (j = 0; j < cfg.dim; j++)
				(*ip)[i][j] = 0;
	
	//std::vector<double*>dL_r; //    [size.size() + hnum][dim]
	for (std::vector<double*>::iterator ip = dL_r.begin(); ip != dL_r.end(); ip++)
		for (j = 0; j < cfg.dim; j++)
			(*ip)[j] = 0;

	//std::vector<double**> dr_e; //  [cfg.enum] x [dim][dim]
	for (std::vector<double**>::iterator ip = dr_e.begin(); ip != dr_e.end(); ip++)
		for (i = 0; i < cfg.dim; i++)
			for (j = 0; j < cfg.dim; j++)
				(*ip)[i][j] = 0;

	//std::vector<double**> dL_e; //  [cfg.e_num] x [dim][dim]
	for (std::vector<double*>::iterator ip = dL_e.begin(); ip != dL_e.end(); ip++)
		for (i = 0; i < cfg.dim; i++)
				(*ip)[i] = 0;

	//std::vector<double**> dr_H; //  [cfg.hnum] x [dim][dim]
	for (std::vector<double**>::iterator ip = dr_H.begin(); ip != dr_H.end(); ip++)
		for (i = 0; i < cfg.dim; i++)
			for (j = 0; j < cfg.dim; j++)
				(*ip)[i][j] = 0;

	//std::vector<double**> dL_H; //  [cfg.hnum] x [dim][dim]
	for (std::vector<double**>::iterator ip = dL_H.begin(); ip != dL_H.end(); ip++)
		for (i = 0; i < cfg.dim; i++)
			for (j = 0; j < cfg.dim; j++)
				(*ip)[i][j] = 0;

	//std::vector<double***> dr_T; // [size.size()] x [dim][dim][dim]
	std::vector<int>::iterator
	is = size.begin();
	for (std::vector<double***>::iterator ip = dr_T.begin(); ip != dr_T.end(); ip++, is++)
		for (i = 0; i < cfg.dim; i++)
			for (j = 0; j < (*is); j++)
				for (k = 0; k < cfg.dim; k++)
					(*ip)[i][j][k] = 0;

	//std::vector<double***> dL_T; // [size.size()] x [dim][dim][dim]
	is = size.begin();
	for (std::vector<double***>::iterator ip = dL_T.begin(); ip != dL_T.end(); ip++, is++)
		for (i = 0; i < cfg.dim; i++)
			for (j = 0; j < (*is); j++)
				for (k = 0; k < cfg.dim; k++)
					(*ip)[i][j][k] = 0;

	return 0;
}

int HIRModel::GradForward(HIRConfig cfg, int* eids, double* feature) {	
	int x, y, z;
	int xx, yy, zz, lh, lr;
	double ztemp, ytemp, *vx, *vy, *vr, ***T, **M;
	xx = zz = cfg.dim;
	//eids[0]; eids[1]; eids[2]; eids[3];

	//T forward
	for (int layer = 0; layer < cfg.size.size(); layer++) {
		yy = cfg.size.at(layer);
		if (layer == 0)// e0, e1
			vx = elist.at(eids[layer]);
		else
			vx = rlist.at(layer - 1);
		if (yy != cfg.dim)//rk, feature
			vy = feature;
		else
			vy = elist.at(eids[layer + 1]);

		T = Tnet.at(layer);
		vr = rlist.at(layer);
		for (z = 0; z < zz; z++) {
			double ztemp, ytemp;
			int x, y;
			ztemp = 0.0;
			for (y = 0; y < yy; y++) {
				ytemp = 0.0;
				for (x = 0; x < xx; x++)
					ytemp += vx[x] * T[z][y][x];
				ztemp += ytemp * vy[y];
			}
			rlist.at(layer)[z] = ztemp; //?
		}
	}
	//TEST
	/*	for (int j = 0; j < cfg.dim; j++)
	printf("%d %.7f\n", j, HIR.rlist.at(0)[j]);

	for (int j = 0; j < cfg.dim; j++)
	printf("%d %.7f\n", j, HIR.rlist.at(1)[j]);

	for (int j = 0; j < cfg.dim; j++)
	printf("%d %.7f\n", j, HIR.rlist.at(2)[j]);

	for (int j = 0; j < cfg.dim; j++)
	printf("%d %.7f\n", j, HIR.rlist.at(3)[j]);*/

	//H forward
	for (int layer = 0; layer < cfg.hnum; layer++) {
		M = hlist.at(layer);
		xx = yy = cfg.dim;
		vx = rlist.at(cfg.size.size() - 1);
		lh = cfg.size.size() + layer;
		for (y = 0; y < yy; y++) {
			vy = M[y];
			double ans = 0;
			for (int i = 0; i < xx; i++) {
				ans += vy[i] * vy[i];
			}
			rlist.at(lh)[y] = ans;
		}
	}
	//TEST
	/*	for (int j = 0; j < cfg.dim; j++)
	printf("%d %.7f\n", j, HIR.rlist.at(4)[j]);
	*/
	return 0;
}

int HIRModel::GradBackward(HIRConfig cfg, HIRTrainParam para, int* index_list) 
{
	int a;

	//H [TESTED]
	for (int h = cfg.hnum - 1; h >= 0; h--) {
		//backward
		a = h + size.size() - 1;
		for (int i = 0; i < cfg.dim; i++) {
			for (int j = 0; j < cfg.dim; j++) {
				dr_H.at(h)[i][j] = rlist.at(a)[j];
				dr_r.at(a)[i][j] = hlist.at(h)[i][j];
			}
		}
		for (int i = 0; i < cfg.dim; i++)
			for (int j = 0; j < cfg.dim; j++) {
				dL_H.at(h)[i][j] = dL_r.at(a + 1)[i] * dr_H.at(h)[i][j];
				hlist.at(h)[i][j] += -1 *para.alpha * dL_H.at(h)[i][j];//UPDATE
				dL_r.at(a)[j] += dL_r.at(a + 1)[i] * dr_r.at(a)[i][j];
			}

	}


	//T backward
	a = cfg.size.size() - 1;
	// feature exist
	if (cfg.fwid != 0) {
		//featrue backward
		for (int i = 0; i < cfg.dim; i++)
			for (int j = 0; j < cfg.fwid; j++)
				for (int k = 0; k < cfg.dim; k++) {
					dr_r.at(a - 1)[i][k] += Tnet.at(a)[i][j][k] * feature[j];
					dr_T.at(a)[i][j][k] += feature[j] * rlist.at(a - 1)[k];
				}

		for (int i = 0; i < cfg.dim; i++)
			for (int j = 0; j < cfg.fwid; j++)
				for (int k = 0; k < cfg.dim; k++) {
					dL_T.at(a)[i][j][k] = dL_r.at(a)[i] * dr_T.at(a)[i][j][k];
					Tnet.at(a)[i][j][k] += -1 * para.alpha * dL_T.at(a)[i][j][k];
				}
		for (int i = 0; i < cfg.dim; i++)
			for (int k = 0; k < cfg.dim; k++)
				dL_r.at(a - 1)[k] = dL_r.at(a)[i] * dr_r.at(a - 1)[i][k];
		a -= 1;
	}

	while (a > 0) {
		for (int i = 0; i < cfg.dim; i++)
			for (int j = 0; j < cfg.fwid; j++)
				for (int k = 0; k < cfg.dim; k++) {
					dr_e.at(a + 1)[i][j] += Tnet.at(a)[i][j][k] * rlist.at(a - 1)[k];
					dr_r.at(a - 1)[i][k] += Tnet.at(a)[i][j][k] * elist.at(index_list[a+1])[j];
					dr_T.at(a)[i][j][k] = elist.at(index_list[a+1])[j] * rlist.at(a - 1)[k];
				}

		for (int i = 0; i < cfg.dim; i++) {
			for (int j = 0; j < cfg.fwid; j++)
				for (int k = 0; k < cfg.dim; k++) {
					dL_T.at(a)[i][j][k] = dL_r.at(a)[i] * dr_T.at(a)[i][j][k];
					Tnet.at(a)[i][j][k] += -1 * para.alpha * dL_T.at(a)[i][j][k];
				}
		}
		for (int i = 0; i < cfg.dim; i++)
			for (int k = 0; k < cfg.dim; k++) {
				dL_r.at(a - 1)[k] += dL_r.at(a)[i] * dr_r.at(a - 1)[i][k];
				dL_e.at(a + 1)[k] += dL_r.at(a)[i] * dr_e.at(a + 1)[i][k];
			}
		for (int i = 0; i < cfg.dim; i++)
			elist.at(index_list[a + 1])[i] += -1 * para.alpha * dL_e.at(a + 1)[i];
		a -= 1;
	}

	if (a == 0) {
		for (int i = 0; i < cfg.dim; i++)
			for (int j = 0; j < cfg.dim; j++)
				for (int k = 0; k < cfg.dim; k++) {
					dr_e.at(a+1)[i][j] += Tnet.at(a)[i][j][k] * elist.at(index_list[a])[k];
					dr_e.at(a)[i][k] += Tnet.at(a)[i][j][k] * elist.at(index_list[a + 1])[j];
					dr_T.at(a)[i][j][k] = elist.at(index_list[a + 1])[j] * elist.at(index_list[a])[k];
				}

		for (int i = 0; i < cfg.dim; i++) {
			elist.at(index_list[a + 1])[i] += -1 * para.alpha * dL_e.at(a + 1)[i];
			for (int j = 0; j < cfg.dim; j++)
				for (int k = 0; k < cfg.dim; k++) {
					dL_T.at(a)[i][j][k] = dL_r.at(a)[i] * dr_T.at(a)[i][j][k];
					Tnet.at(a)[i][j][k] += -1 * para.alpha * dL_T.at(a)[i][j][k];
				}
		}
		for (int i = 0; i < cfg.dim; i++)
			for (int k = 0; k < cfg.dim; k++) {
				dL_e.at(a + 1)[k] += dL_r.at(a)[i] * dr_e.at(a + 1)[i][k];
				dL_e.at(a)[k] += dL_r.at(a)[i] * dr_e.at(a)[i][k];
			}
		for (int i = 0; i < cfg.dim; i++) {
			elist.at(index_list[a + 1])[i] += -1 * para.alpha * dL_e.at(a + 1)[i];
			elist.at(index_list[a])[i] += -1 * para.alpha * dL_e.at(a)[i];
		}
	}
	return 0; 
}