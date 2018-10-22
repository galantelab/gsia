#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <hyper.h>
#include <error.h>

#define N_DIGITS 10

int
main (int argc, char *argv[])
{
	if (argc != 5)
		error (1, set_error_code (D_LOCAL, E_NOARG), "Usage %s N K n k", PACKAGE);

	int N = atoi (argv[1]);
	int K = atoi (argv[2]);
	int n = atoi (argv[3]);
	int k = atoi (argv[4]);

	hyper_s h;
	hyper (&h, N, K, n, k);

	printf ("[N=%d K=%d n=%d k=%d]\n", N, K, n, k);
	printf ("P(N eq %d) = %.*lf\n", k, N_DIGITS, h.pmf);
	printf ("P(N lt %d) = %.*lf\n", k, N_DIGITS, h.cdfe);
	printf ("P(N le %d) = %.*lf\n", k, N_DIGITS, h.cdfi);
	printf ("P(N gt %d) = %.*lf\n", k, N_DIGITS, 1 - h.cdfi);
	printf ("P(N ge %d) = %.*lf\n", k, N_DIGITS, 1 - h.cdfe);

	return 0;
}
