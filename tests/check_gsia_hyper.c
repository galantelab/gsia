#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "../src/hyper.h"
#include <check.h>
#include <check_gsia.h>

static int
fequal (double a, double b)
{
	double precision = 0.00001;
	return (a - precision) < b && (a + precision) > b;
}

START_TEST (test_hyper)
{
	hyper_s h;

	int N = 50;
	int K = 5;
	int n = 10;
	int k = 4;

	hyper (&h, N, K, n, k);
	ck_assert (fequal (h.pmf, 0.0039645831));
	ck_assert (fequal (h.cdfe, 0.9959164795));
	ck_assert (fequal (h.cdfi, 0.9998810625));
	ck_assert (fequal (1 - h.cdfi, 0.0001189375));
	ck_assert (fequal (1 - h.cdfe, 0.0040835205));
}
END_TEST

Suite *
make_hyper_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("Hyper");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_hyper);
	suite_add_tcase (s, tc_core);

	return s;
}
