#ifndef HYPER_H
#define HYPER_H

struct hyper_s
{
	double pmf;
	double cdfe;
	double cdfi;
};

typedef struct hyper_s hyper_s;

void hyper (hyper_s *h, int N, int K, int n, int k);

#endif /* hyper.h */
