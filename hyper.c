#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <assert.h>

#define N_DIGITS 10
#define error(...) do { fprintf (stderr, __VA_ARGS__);  exit (1); } while (0)

void
fact (mpf_t result, int n)
{
	assert (n >= 0);

	mpf_set_ui (result, 1);

	for (int i = 2; i <= n; i++)
		mpf_mul_ui (result, result, i);
}

void
choose (mpf_t result, int n, int p)
{
	assert (n >= 0 && p >= 0 && n >= p);

	mpf_t fp, fnp;
	mpf_inits (fp, fnp, NULL);

	fact (result, n);
	fact (fp, p);
	fact (fnp, n - p);

	mpf_mul (fp, fp, fnp);
	mpf_div (result, result, fp);

	mpf_clears (fp, fnp, NULL);
}

void
phyper (mpf_t result, int N, int K, int n, int k)
{
	mpf_t cnk, cnn;
	mpf_inits (cnk, cnn, NULL);

	choose (result, K, k);
	choose (cnk, N - K, n - k);
	choose (cnn, N, n);

	mpf_mul (result, result, cnk);
	mpf_div (result, result, cnn);

	mpf_clears (cnk, cnn, NULL);
}

void
phyper_cdf (mpf_t result, int N, int K, int n, int k)
{
	assert (k >= 0);

	mpf_t tmp;
	mpf_init (tmp);

	for (int i = 0; i <= k; i++)
		{
			phyper (tmp, N, K, n, i);
			mpf_add (result, result, tmp);
		}

	mpf_clear (tmp);
}

int
main (int argc, char *argv[])
{
	if (argc != 5)
		error ("Usage %s N K n k\n", argv[0]);

	int N = atoi (argv[1]);
	int K = atoi (argv[2]);
	int n = atoi (argv[3]);
	int k = atoi (argv[4]);

	mpf_t pmf, cdfi, cdfe;
	mpf_inits (pmf, cdfi, cdfe, NULL);

	phyper (pmf, N, K, n, k);
	phyper_cdf (cdfi, N, K, n, k);
	phyper_cdf (cdfe, N, K, n, k - 1);

	gmp_printf ("[N=%d K=%d n=%d k=%d]\n", N, K, n, k);
	gmp_printf ("P(N eq %d) = %.*Ff\n", k, N_DIGITS, pmf);
	gmp_printf ("P(N lt %d) = %.*Ff\n", k, N_DIGITS, cdfe);
	gmp_printf ("P(N le %d) = %.*Ff\n", k, N_DIGITS, cdfi);
	gmp_printf ("P(N gt %d) = %.*lf\n", k, N_DIGITS, 1 - mpf_get_d (cdfi));
	gmp_printf ("P(N ge %d) = %.*lf\n", k, N_DIGITS, 1 - mpf_get_d (cdfe));

	mpf_clears (pmf, cdfe, cdfi, NULL);
	return 0;
}
