#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <gmp.h>
#include <assert.h>
#include <hyper.h>

static void
fact (mpf_t result, int n)
{
	assert (n >= 0);

	mpf_set_ui (result, 1);

	for (int i = 2; i <= n; i++)
		mpf_mul_ui (result, result, i);
}

static void
choose (mpf_t result, int n, int p)
{
	/*printf ("n = %d p = %d\n", n, p);*/
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

static void
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

// N >= K n >= k N >= n (N-K) >= (n-k)
// Mayb return until k and k-1
static void
phyper_cdf (mpf_t result, int N, int K, int n, int k)
{
	assert (k >= 0);

	mpf_t tmp;
	mpf_init (tmp);

	for (int i = 0; i <= k; i++)
		{
			/*if ((N - K) < (n - i)) continue;*/
			phyper (tmp, N, K, n, i);
			mpf_add (result, result, tmp);
		}

	mpf_clear (tmp);
}

void
hyper (hyper_s *h, int N, int K, int n, int k)
{
	assert (h != NULL);

	mpf_t pmf, cdfi, cdfe;
	mpf_inits (pmf, cdfi, cdfe, NULL);

	phyper (pmf, N, K, n, k);
	phyper_cdf (cdfi, N, K, n, k);
	phyper_cdf (cdfe, N, K, n, k > 0 ? k - 1 : k);

	*h = (hyper_s){
		.pmf  = mpf_get_d (pmf),
		.cdfi = mpf_get_d (cdfi),
		.cdfe = mpf_get_d (cdfe)
	};

	mpf_clears (pmf, cdfi, cdfe, NULL);
}
