#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include "check_gsia.h"

#include "../src/io.h"

static char *
create_tmpfile (size_t nlines, const char * const *lines)
{
	char *filename = strdup ("/tmp/tmpfile-XXXXXX");
	mktemp (filename);

	FILE *fp = fopen (filename, "w");

	for (size_t i = 0; i < nlines; i++)
		fprintf (fp, "%s\n", lines[i]);

	fclose (fp);
	return filename;
}

START_TEST (test_read_file_lines)
{
	size_t nlines = 5;
	const char *lines[] = {
		"ponga0",
		"ponga1",
		"ponga2",
		"ponga3",
		"ponga4"
	};

	char *filename = create_tmpfile (nlines, lines);

	PtrArray *arr = ptr_array_new (NULL, NULL);
	read_file_lines (arr, filename);

	ck_assert_int_eq (arr->len, nlines);

	for (int i = 0; i < nlines; i++)
		ck_assert_str_eq ((char *) ptr_array_get (arr, i), lines[i]);

	ptr_array_free (arr, 1);
	remove (filename);
	free (filename);
}
END_TEST

Suite *
make_io_suite (void)
{
	Suite *s;
	TCase *tc_core;

	s = suite_create ("IO");

	/* Core test case */
	tc_core = tcase_create ("Core");

	tcase_add_test (tc_core, test_read_file_lines);
	suite_add_tcase (s, tc_core);

	return s;
}
