#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "check_gsia.h"

#include "../src/comm.h"

static int
cmpstringp (const void *p1, const void *p2)
{
	return strcmp (* (char * const *) p1, * (char * const *) p2);
}

START_TEST (test_comm)
{
	Comm *comm = NULL;
	PtrArray *array1 = NULL;
	PtrArray *array2 = NULL;

	array1 = ptr_array_new (NULL, NULL);
	array2 = ptr_array_new (NULL, NULL);

	int data_len = 5;
	char *data1[] = { "a", "b", "c", "d", "e" };
	char *data2[] = { "c", "d", "e", "f", "g" };
	char *uniq1[] = { "a", "b" };
	char *uniq2[] = { "f", "g" };
	char *shared[] = { "c", "d", "e" };

	for (int i = 0; i < data_len; i++)
		{
			ptr_array_add (array1, data1[i]);
			ptr_array_add (array2, data2[i]);
		}

	ptr_array_sort (array1, cmpstringp);
	ptr_array_sort (array2, cmpstringp);

	comm = compare_arrays (array1, array2, cmpstringp, NULL);

	for (int i = 0; i < comm->uniq_to_file1->len; i++)
		ck_assert_str_eq (ptr_array_get (comm->uniq_to_file1, i), uniq1[i]);

	for (int i = 0; i < comm->uniq_to_file2->len; i++)
		ck_assert_str_eq (ptr_array_get (comm->uniq_to_file2, i), uniq2[i]);

	for (int i = 0; i < comm->shared->len; i++)
		ck_assert_str_eq (ptr_array_get (comm->shared, i), shared[i]);

	comm_free (comm);
	ptr_array_free (array1, 1);
	ptr_array_free (array2, 1);
}
END_TEST

Suite *
make_comm_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("Comm");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_comm);
	suite_add_tcase (s, tc_core);

	return s;
}
