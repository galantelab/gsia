#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <check.h>
#include <check_gsia.h>

#include "../src/array.h"

START_TEST (test_array)
{
	PtrArray *arr = ptr_array_new (free, NULL);
	int ponga_size = 5;

	char *ponga[] = {
		"Ponga1",
		"Ponga2",
		"Ponga3",
		"Ponga4",
		"Ponga5"
	};

	for (int i = 0; i < ponga_size; i++)
		ptr_array_add (arr, strdup (ponga[i]));

	ck_assert_int_eq (arr->len, ponga_size);

	for (int i = 0; i < arr->len; i++)
		ck_assert_str_eq ((char *) ptr_array_get (arr, i), ponga[i]);

	ptr_array_free (arr, 1);
}
END_TEST

START_TEST (test_array_no_free_segment)
{
	PtrArray *arr = ptr_array_new (free, NULL);
	int ponga_size = 5;

	char *ponga[] = {
		"Ponga1",
		"Ponga2",
		"Ponga3",
		"Ponga4",
		"Ponga5"
	};

	for (int i = 0; i < ponga_size; i++)
		ptr_array_add (arr, strdup (ponga[i]));

	char **ponga2 = (char **)ptr_array_free (arr, 0);

	for (int i = 0; i < ponga_size; i++)
		ck_assert_str_eq (ponga2[i], ponga[i]);

	for (int i = 0; i < ponga_size; i++)
		free (ponga2[i]);

	free (ponga2);
}
END_TEST

static int
cmpstringp (const void *p1, const void *p2)
{
	return strcmp (* (char * const *) p1, * (char * const *) p2);
}

START_TEST (test_array_uniq)
{
	PtrArray *arr = ptr_array_new (free, NULL);
	int ponga_size = 15;

	char *ponga[] = {
		"Ponga1",
		"Ponga1",
		"Ponga5",
		"Ponga5",
		"Ponga5",
		"Ponga1",
		"Ponga1",
		"Ponga2",
		"Ponga3",
		"Ponga4",
		"Ponga4",
		"Ponga4",
		"Ponga4",
		"Ponga5",
		"Ponga5"
	};

	int uniq_size = 5;
	char *uniq[] = {
		"Ponga1",
		"Ponga2",
		"Ponga3",
		"Ponga4",
		"Ponga5",
	};

	for (int i = 0; i < ponga_size; i++)
		ptr_array_add (arr, strdup (ponga[i]));

	ptr_array_uniq (arr, cmpstringp);
	ck_assert_int_eq (arr->len, uniq_size);

	for (int i = 0; i < arr->len; i++)
		ck_assert_str_eq ((char *) ptr_array_get (arr, i), uniq[i]);
}
END_TEST

Suite *
make_array_suite (void)
{
	Suite *s;
	TCase *tc_core;
	TCase *tc_free;

	s = suite_create ("Array");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_array);
	tcase_add_test (tc_core, test_array_uniq);
	suite_add_tcase (s, tc_core);

	/* Free test case */
	tc_free = tcase_create ("Free");

	tcase_add_test (tc_free, test_array_no_free_segment);
	suite_add_tcase (s, tc_free);

	return s;
}
