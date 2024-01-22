#ifndef SYMNMF_H_
#define SYMNMF_H_
double **sym(double **X);
double **ddg(double **X);
double **norm(double **X);
double avgToPy(double **X);
double **symnmf(double **H0, double **W);
void printMat(double **mat, int rows, int cols);
void updateGlobals(int num, int d, int k);
#endif