#pragma once
#include<math.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<iterator> //back_inserter
#include<string> // for string operator comparision
#include<cstdlib>
#include<vector>
#include <algorithm> // std::move_backward  
#include <random> // std::default_random_engine  
#include <chrono> // std::chrono::system_clock 
#include "HIRArgument.h"
#include "HIRModel.h"
#include "HIRRecord.h"
#include"HIRConfig.h"

static HIRConfig cfg;
static HIRModel HIR;

double t_slides(int, int, int, int, double***, double*, double*);
double dotMul(int dim, double* a, double* b); //vec * vec = num

double evaluate(int, int, double**, double**);

double loss(int, double*, double*);

double sigmoid(double x);

double d_sigmoid(double x);

double square(double x);

//Evaluation Recall@1 for Classification
double recall(int* dream, int* reality, int dim);
//Evaluation precise for Classification
double precise(int* dream, int* reality, int dim);

int Toutmul(double* v1, double* v2, int dim1, int dim2, int dim3, double****to_tensor);

int Tsummul(double* v1, double****tensor, bool isleftv, int dim1, int dim2, int dim3, double***to_matrix);

int Mtrans(double***from_matrix, int dim1, int dim2, double***to_matrix);

int Mcat(double***mat1, double***mat2, int dim, int dim1, int dim2, double***to_matrix);