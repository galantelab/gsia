#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"
#include "hyper.h"

void
hyper_free (Hyper *h)
{
	if (h == NULL)
		return;
	free (h);
}

Hyper *
hyper (int N, int K, int n, int k, int *errnum)
{
	assert (N >= 0 && K >= 0 && n >= 0 && k >= 0);
	assert (K <= N && n <= N && k <= n);
	assert (errnum == NULL || *errnum == 0);

	Hyper *h = NULL;
	double pmf = 0.0f;
	double cdfi_P = 0.0f;
	double cdfe_P = 0.0f;

	h = calloc (1, sizeof (Hyper));
	if (h == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			return NULL;
		}

	pmf = gsl_ran_hypergeometric_pdf (k, K, N - K, n);
	cdfi_P = gsl_cdf_hypergeometric_P (k, K, N - K, n);
	cdfe_P = cdfi_P - pmf;

	*h = (Hyper){
		.pmf    = pmf,
		.cdfe_P = cdfe_P,
		.cdfi_P = cdfi_P,
		.cdfe_Q = 1 - cdfi_P,
		.cdfi_Q = 1 - cdfe_P
	};

	return h;
}
