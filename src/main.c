#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hyper.h"
#include "array.h"
#include "comm.h"
#include "io.h"
#include "error.h"

#define N_DIGITS 20

static int
cmpstringp (const void *p1, const void *p2)
{
	return strcasecmp (* (char * const *) p1, * (char * const *) p2);
}

int
main (int argc, char *argv[])
{
	if (argc == 1)
		return 0;

	// Precisa-se de 3 arquivos:
	// A: Universe
	// B: Max Success
	// C: Observed Success
	int rc = 0;
	Comm *comm1_2 = NULL;
	Comm *comm2_3 = NULL;
	Comm *comm1_3 = NULL;
	Hyper *h = NULL;
	PtrArray *array1 = NULL;
	PtrArray *array2 = NULL;
	PtrArray *array3 = NULL;

	array1 = ptr_array_new (free, &rc);
	array2 = ptr_array_new (free, &rc);
	array3 = ptr_array_new (free, &rc);

	if (rc != E_SUCCESS)
		{
			error (0, rc, "Failed to allocate resources");
			goto Exit;
		}

	rc = read_file_lines (array1, argv[1]);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Failed to read file '%s'", argv[1]);
			goto Exit;
		}

	if (array1->len == 0)
		{
			rc = set_error_code (D_LOCAL, E_NOLINE);
			error (0, rc, "Failed to read file '%s'", argv[1]);
			goto Exit;
		}

	rc = read_file_lines (array2, argv[2]);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Failed to read file '%s'", argv[2]);
			goto Exit;
		}

	if (array2->len == 0)
		{
			rc = set_error_code (D_LOCAL, E_NOLINE);
			error (0, rc, "Failed to read file '%s'", argv[2]);
			goto Exit;
		}

	rc = read_file_lines (array3, argv[3]);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Failed to read file '%s'", argv[3]);
			goto Exit;
		}

	if (array3->len == 0)
		{
			rc = set_error_code (D_LOCAL, E_NOLINE);
			error (0, rc, "Failed to read file '%s'", argv[3]);
			goto Exit;
		}

	ptr_array_uniq (array1, cmpstringp);
	ptr_array_uniq (array2, cmpstringp);
	ptr_array_uniq (array3, cmpstringp);

	comm1_2 = compare_arrays (array1, array2, cmpstringp, &rc);
	if (rc != E_SUCCESS)
		{
			assert (comm1_2 == NULL);
			error (0, rc, "Error during comparing files '%s' and '%s'", argv[1], argv[2]);
			goto Exit;
		}

	comm1_3 = compare_arrays (array1, array3, cmpstringp, &rc);
	if (rc != E_SUCCESS)
		{
			assert (comm1_3 == NULL);
			error (0, rc, "Error during comparing files '%s' and '%s'", argv[1], argv[3]);
			goto Exit;
		}

	comm2_3 = compare_arrays (comm1_2->shared, comm1_3->shared, cmpstringp, &rc);
	if (rc != E_SUCCESS)
		{
			assert (comm2_3 == NULL);
			error (0, rc, "Error during comparing files '%s' and '%s'", argv[2], argv[3]);
			goto Exit;
		}

	int N = array1->len;
	int K = comm1_2->shared->len;
	int n = comm1_3->shared->len;
	int k = comm2_3->shared->len;

	h = hyper (N, K, n, k, &rc);
	if (rc != E_SUCCESS)
		{
			assert (h == NULL);
			error (0, rc, "Error during hypergeometric test");
			goto Exit;
		}

	printf ("*******************************************************\n");
	printf (" %s\n", PACKAGE_STRING);
	printf (" [universe]\n");
	printf ("\tFile: '%s'\n", argv[1]);
	printf (" \tTotal: %zu\n", array1->len);

	printf (" [feature]\n");
	printf ("\tFile: '%s'\n", argv[2]);
	printf (" \tTotal: %zu\n", array2->len);
	printf (" \tShared with universe: %zu (%.2f%%)\n", comm1_2->shared->len,
			(float) comm1_2->shared->len * 100 / array2->len);

	printf (" [list]\n");
	printf ("\tFile: '%s'\n", argv[3]);
	printf (" \tTotal: %zu\n", array3->len);
	printf (" \tShared with universe: %zu (%.2f%%)\n", comm1_3->shared->len,
			(float) comm1_3->shared->len * 100 / array3->len);
	printf (" \tShared with universe and feature: %zu (%.2f%%)\n", comm2_3->shared->len,
			(float) comm2_3->shared->len * 100 / array3->len);
	printf ("*******************************************************\n\n");

	printf ("[N=%d K=%d n=%d k=%d]\n", N, K, n, k);
	printf ("P(N eq %d) = %.*lf\n", k, N_DIGITS, h->pmf);
	printf ("P(N lt %d) = %.*lf\n", k, N_DIGITS, h->cdfe_P);
	printf ("P(N le %d) = %.*lf\n", k, N_DIGITS, h->cdfi_P);
	printf ("P(N gt %d) = %.*lf\n", k, N_DIGITS, h->cdfe_Q);
	printf ("P(N ge %d) = %.*lf\n", k, N_DIGITS, h->cdfi_Q);

Exit:
	comm_free (comm1_2);
	comm_free (comm2_3);
	comm_free (comm1_3);
	hyper_free (h);
	ptr_array_free (array1, 1);
	ptr_array_free (array2, 1);
	ptr_array_free (array3, 1);
	return rc == E_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
