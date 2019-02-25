#ifndef HYPER_H
#define HYPER_H

struct _Hyper
{
	double pmf;
	double cdfe_P;
	double cdfi_P;
	double cdfe_Q;
	double cdfi_Q;
};

typedef struct _Hyper Hyper;

Hyper * hyper      (int N, int K, int n, int k, int *errnum);
void    hyper_free (Hyper *h);

#endif /* hyper.h */
