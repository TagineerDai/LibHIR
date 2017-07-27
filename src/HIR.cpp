#include "HIR.h"

double t_slides(int zz, int yy, int xx, int z, double***T, double*vx, double*vy) {
	double ztemp, ytemp;
	int x, y;
	ztemp = 0.0;
	for (y = 0; y < yy; y++) {
		ytemp = 0.0;
		for (x = 0; x < xx; x++)
			ytemp += vx[x]* T[z][y][x];
		ztemp += ytemp * vy[y];
	}
	return ztemp;
}


double evaluate(int category, int rnum, double** y, double** y_hat) {
	double ee = 0;
	switch (category) {
	case ABS_E: {
		//std::cout << "ABS_E" << std::endl;
		for (int i = 0; i < rnum; i++) {
			ee += abs(y[i][0] - y_hat[i][0]);
		}
		ee /= rnum;
		break;
	}
	case RMSE_E: {
		//std::cout << "RMSE_E" << std::endl;
		for (int i = 0; i < rnum; i++) {
			ee += square(y[i][0] - y_hat[i][0]);
		}
		ee /= rnum;
		ee = sqrt(ee);
		break;
	}
	case ACC_E: {
		ee = 0;
		//std::cout << "ACC_E" << std::endl;
		int yans, yhatans;
		double yv, yhatv;
		for (int i = 0; i < rnum; i++) {
			yv = -1e5; yhatv = -1e5;
			yans = 0, yhatans = 0;
			for (int j = 0; j < cfg.task; j++) {
				if (yv < y[i][j]) {
					yv = y[i][j]; yans = j;
				}
				if (yhatv < y_hat[i][j]) {
					yhatv = y_hat[i][j]; yhatans = j;
				}
			}
			if (yans != yhatans)
				ee = ee + 1;
		}
		ee /= rnum;
		break;
	}
	case AUC_E: {
		ee = 0;
		double temp;
		//std::cout << "AUC_E" << std::endl;
		for (int r = 0; r < rnum; r++) {
			//regularization
			double may = -1e20, miy = 1e20;
			for (int i = 0; i < cfg.task; i++) {
				may = y_hat[r][i] > may ? y_hat[r][i] : may;
				miy = y_hat[r][i] < miy ? y_hat[r][i] : miy;
			}
			for (int i = 0; i < cfg.task; i++) {
				y_hat[r][i] = (y_hat[r][i] - miy) / (may - miy);
			}
			//sorted by y 
			for (int i = 0; i < cfg.task-1; i++) {
				for (int j = 0; j < cfg.task - i-1; j++) {
					if (y[r][j+1] < y[r][j]) {
						temp = y[r][j+1];
						y[r][j+1] = y[r][j];
						y[r][j] = temp;
						temp = y_hat[r][j+1];
						y_hat[r][j+1] = y_hat[r][j];
						y_hat[r][j] = temp;
					}
				}
			}

			for (int i = 1; i < cfg.task; i++)
					ee += (y[r][i] - y[r][i - 1])* y_hat[r][i];
			ee += (1 - y[r][cfg.task - 1]) * 1;
			//TODO er;
		}
		ee /= rnum;
		break;
	}
	}//evaluate switch
	return ee;
}

double loss(int category, double*y, double*y_hat) {
	double ee;
	ee = 0;
	switch (category) {
	case RMSE_L: {
		for (int i = 0; i < cfg.task; i++)
			ee = ee + square(y[i] - y_hat[i]);
		ee = ee / cfg.task;
		ee = sqrt(ee);
		break; 
	}
	case ABS_L: {
		for (int i = 0; i < cfg.task; i++)
			ee += abs(y[i] - y_hat[i]);
		ee = ee / cfg.task;
		break;
	}
	case LL_L: {
		ee = 0;
		for (int i = 0; i < cfg.task; i++)
			ee += y[i] * log(y_hat[i]) + (1 - y[i]) *log(1 - y_hat[i]);
		ee /= cfg.task;
		break;
	}
	case GOLD_L: {
		ee = 0;
		int yans = 0, yhatans = 0;
		double vy = -1e5, vhaty = -1e5;
		for (int i = 0; i < cfg.task; i++) {
			if (vy < y[i]) {
				vy = y[i]; 
				yans = i;
			}
			if (vhaty < y_hat[i]) {
				vhaty = y_hat[i]; 
				yhatans = i;
			}
		}
		if (yans != yhatans)
			ee = 1;
		break;
	}
	}
	return ee;
}

/*
void forward(int index, HIRModel* HIR, HIRRecord* REC, HIRConfig cfg) {
	int* eids = REC->ilist.at(index);
	int x, y, z;
	int xx, yy, zz, layer;
	double ztemp, ytemp, *vx, *vy, *vr, ***T;
	xx = zz = cfg.dim;

	//e0, e1, T0 --> r0
	layer = 0;
	yy = cfg.size.at(layer);
	vx = HIR->elist.at(eids[layer]);
	vy = HIR->elist.at(eids[layer + 1]);
	T = HIR->Tnet.at(layer);
	vr = HIR->rlist.at(layer);
	for (z = 0; z < zz; z++) {
		ztemp = 0.0;
		for (y = 0; y < yy; y++) {
			ytemp = 0.0;
			for (x = 0; x < xx; x++)
				ytemp += vx[x] * T[z][y][x];
			ztemp += ytemp * vy[y];
		}
		vr[z] = ztemp;
	}
	layer += 1;
	
	int lastT = cfg.e_num - 1;
	if (cfg.fwid) lastT -= 1;
	
	while (layer < lastT) {
		yy = cfg.size.at(layer);
		vx = HIR->rlist.at(layer - 1);
		vy = HIR->elist.at(eids[layer + 1]);
		T = HIR->Tnet.at(layer);
		vr = HIR->rlist.at(layer);
		for (z = 0; z < zz; z++) {
			ztemp = 0;
			for (y = 0; y < yy; y++) {
				ytemp = 0;
				for (x = 0; x < xx; x++)
					ytemp += vx[x] * T[z][y][x];
				ztemp += ytemp * vy[y];
			}
			vr[z] = ztemp;
		}
		layer += 1;
	}
	if (cfg.fwid) {
		yy = cfg.size.at(layer);
		vx = HIR->rlist.at(layer - 1);
		vy = HIR->elist.at(eids[layer + 1]);
		T = HIR->Tnet.at(layer);
		vr = HIR->rlist.at(layer);
		for (z = 0; z < zz; z++) {
			ztemp = 0;
			for (y = 0; y < yy; y++) {
				ytemp = 0;
				for (x = 0; x < xx; x++)
					ytemp += vx[x] * T[z][y][x];
				ztemp += ytemp * vy[x];
			}
			vr[z] = ztemp;
		}
	}
	return;
}
*/
double sigmoid(double x) {
	return 1.0 / (1.0 + exp(-1.0 * x));
}

double square(double x) {
	return x*x;
}

double d_sigmoid(double x){
	double fx = 1.0 / (1.0 + exp(-1.0*x));
	return fx*(1 - fx);
}

void cpydftr(int dim, double* to, double* from) {
	for (int i = 0; i > dim; i++)
		to[i] = from[i];
	return;
}

//Multiple reloadd
void forwardM(int xx, int yy, double* to, double* from, double* hir, double** trans) {
	//forward  (from | hir)[xx]* trans[yy][xx] = to[yy] 
	int i, j;
	for (i = 0; i < yy; i++) {
		to[i] = 0;
		for (j = 0; j < yy; j++)
			to[i] += from[j] * trans[i][j];
		for (j = 0; j < xx - yy; j++)
			to[i] += hir[j] * trans[i][j + yy];
	}
	return;
}

void forwardH(int xx, double* to, double* from, double** trans) {
	//forward from[xx] * trans[xx1][xx] = to[xx1]
	int i, j;
	for (i = 0; i < xx; i++) {
		to[i] = 0;
		for (j = 0; j < xx; j++)
			to[i] += from[j] * trans[i][j];
	}
	return;
}

void forwardT(int xx, int yy, double* to, double* from, double* hir, double*** trans) {
	//forward for every k to[i] = from[] * trans[i][][] * hir[]
	int i, j, k;
	int zz = xx;
	double* buf = new double[yy];
	for (i = 0; i < zz; i++) {
		to[i] = 0;
		for (j = 0; j < yy; j++) {
			buf[j] = 0;
			for (k = 0; k < xx; k++) {
				buf[j] += trans[i][j][k] * from[k];
			}
			to[i] += buf[j] * hir[j];
		}
	}
	return;
}

double dotMul(int dim, double* a, double* b) {
	double ans = 0;
	for (int i = 0; i < dim; i++) {
		ans += a[i] * b[i];
	}
	return ans;
}


//multi with a const
void cMul(double** to, double* from, double para, int dim) {
	for (int i = 0; i < dim; i++)
		(*to)[i] = para * from[i];
	return;
}

//multi 2 vector into a matrix
void outMul(double* va, double* vb, double*** mat, int dima, int dimb) {
	for (int i = 0; i < dima; i++) {
		for (int j = 0; j < dimb; j++) {
			(*mat)[i][j] = va[i] * vb[j];
		}
	}
	return;
}

//Evaluation Recall@1 for Classification
/*
double recall(int* dream, int* reality, int dim) {
	int hit = 0, tot = 0;
	for (int i = 0; i < dim; i++) {
		if (dream[i] != 0) {
			if (reality[i] != 0) hit++;
			tot++;
		}
	}
	return ((double)hit / (double)tot);
}*/
//Evaluation precise for Classification
double precise(int* dream, int* reality, int dim) {
	int hit = 0;
	for (int i = 0; i < dim; i++)
		if (dream[i] == reality[i]) hit++;
	return ((double)hit / (double)dim);
}

int Toutmul(double* v1, double* v2, int dim1, int dim2, int dim3, double****to_tensor) {
	for (int i = 0; i < dim1; i++)
		for (int j = 0; j < dim2; j++)
			for (int l = 0; l < dim3; l++)
				(*to_tensor)[l][i][j] = v1[i] * v2[j];
	return 0;
}

int Tsummul(double* v1, double****tensor, bool isleftv, int dim1, int dim2, int dim3, double***to_matrix) {
	for (int i = 0; i < dim1; i++)
		for (int j = 0; j < dim2; j++) {
			(*to_matrix)[i][j] = 0;
			for (int l = 0; l < dim3; l++) {
				if (isleftv) {//vector * matrix
					(*to_matrix)[i][j] += v1[i] * (*tensor)[l][i][j]; //Check index
				}
				else {//matrix * vector
					(*to_matrix)[i][j] += (*tensor)[l][i][j] * v1[j]; //Check index
				}
			}
		}
	return 0;
}

int Mtrans(double***from_matrix, int dim1, int dim2, double***to_matrix) {
	for (int i = 0; i < dim1; i++) {
		for (int j = 0; j < dim2; j++) {
			(*to_matrix)[j][i] = (*from_matrix)[i][j];
		}
	}
	return 0;
}

int Mcat(double***mat1, double***mat2, int dim, int dim1, int dim2, double***to_matrix) {
	for (int i = 0; i < dim1; i++) {
		for (int j = 0; j < dim; j++) {
			(*to_matrix)[i][j] = (*mat1)[i][j];
		}
	}
	for (int i = 0; i < dim2; i++) {
		for (int j = 0; j < dim; j++) {
			(*to_matrix)[i + dim1][j] = (*mat2)[i][j];
		}
	}
	return 0;
}
