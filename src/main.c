#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hyper.h>
#include <array.h>
#include <comm.h>
#include <io.h>
#include <error.h>
#include <assert.h>

#define N_DIGITS 10

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
	PtrArray *array1 = NULL;
	PtrArray *array2 = NULL;
	PtrArray *array3 = NULL;

	comm1_2 = comm_new (NULL, &rc);
	comm2_3 = comm_new (NULL, &rc);
	comm1_3 = comm_new (NULL, &rc);
	array1 = ptr_array_new (free, &rc);
	array2 = ptr_array_new (free, &rc);
	array3 = ptr_array_new (free, &rc);

	if (rc != E_SUCCESS)
		{
			error (0, rc, "Failed to allocate resources");
			goto Exit;
		}

	assert (rc == 0);

	rc = read_file_lines (array1, argv[1]);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Failed to read file '%s'", argv[1]);
			goto Exit;
		}

	assert (rc == 0);

	if (array1->len == 0)
		{
			rc = set_error_code (D_LOCAL, E_NOLINE);
			error (0, rc, "Failed to read file '%s'", argv[1]);
			goto Exit;
		}

	assert (rc == 0);

	rc = read_file_lines (array2, argv[2]);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Failed to read file '%s'", argv[2]);
			goto Exit;
		}

	assert (rc == 0);

	if (array2->len == 0)
		{
			rc = set_error_code (D_LOCAL, E_NOLINE);
			error (0, rc, "Failed to read file '%s'", argv[2]);
			goto Exit;
		}

	assert (rc == 0);

	rc = read_file_lines (array3, argv[3]);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Failed to read file '%s'", argv[3]);
			goto Exit;
		}

	assert (rc == 0);

	if (array3->len == 0)
		{
			rc = set_error_code (D_LOCAL, E_NOLINE);
			error (0, rc, "Failed to read file '%s'", argv[3]);
			goto Exit;
		}

	assert (rc == 0);

	ptr_array_uniq (array1, cmpstringp);
	ptr_array_uniq (array2, cmpstringp);
	ptr_array_uniq (array3, cmpstringp);

	rc = compare_arrays (comm1_2, array1, array2, cmpstringp);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Error during comparing files '%s' and '%s'", argv[1], argv[2]);
			goto Exit;
		}

	rc = compare_arrays (comm1_3, array1, array3, cmpstringp);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Error during comparing files '%s' and '%s'", argv[1], argv[3]);
			goto Exit;
		}

	rc = compare_arrays (comm2_3, comm1_2->shared, comm1_3->shared, cmpstringp);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Error during comparing files '%s' and '%s'", argv[2], argv[3]);
			goto Exit;
		}

	/*printf ("Uniq lines to %s\n", argv[1]);*/
	/*for  (int i = 0;  i < comm->uniq_to_file1->len; i++)*/
		/*printf ("- %s\n", (char *) ptr_array_get (comm1_2->uniq_to_file1, i));*/

	/*printf ("Uniq lines to %s\n", argv[2]);*/
	/*for  (int i = 0;  i < comm->uniq_to_file2->len; i++)*/
		/*printf ("- %s\n", (char *) ptr_array_get (comm1_2->uniq_to_file2, i));*/

	/*printf ("Shared lines between %s and %s\n", argv[1], argv[2]);*/
	/*for  (int i = 0;  i < comm->shared->len; i++)*/
		/*printf ("- %s\n", (char *) ptr_array_get (comm1_2->shared, i));*/

	int N = array1->len;
	int K = comm1_2->shared->len;
	int n = comm1_3->shared->len;
	int k = comm2_3->shared->len;

	hyper_s h;
	hyper (&h, N, K, n, k);

	printf ("[N=%d K=%d n=%d k=%d]\n", N, K, n, k);
	printf ("P(N eq %d) = %.*lf\n", k, N_DIGITS, h.pmf);
	printf ("P(N lt %d) = %.*lf\n", k, N_DIGITS, h.cdfe);
	printf ("P(N le %d) = %.*lf\n", k, N_DIGITS, h.cdfi);
	printf ("P(N gt %d) = %.*lf\n", k, N_DIGITS, 1 - h.cdfi);
	printf ("P(N ge %d) = %.*lf\n", k, N_DIGITS, 1 - h.cdfe);

Exit:
	comm_free (comm1_2, 1);
	comm_free (comm2_3, 1);
	comm_free (comm1_3, 1);
	ptr_array_free (array1, 1);
	ptr_array_free (array2, 1);
	ptr_array_free (array3, 1);
	return rc == E_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
