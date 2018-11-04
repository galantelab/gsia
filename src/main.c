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

	int rc = 0;
	Comm *comm = NULL;
	PtrArray *array1 = NULL;
	PtrArray *array2 = NULL;
	/*PtrArray *array3 = NULL;*/

	comm = comm_new (free, &rc);
	array1 = ptr_array_new (free, &rc);
	array2 = ptr_array_new (free, &rc);
	/*array3 = ptr_array_new (free, &rc);*/

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

	/*rc = read_file_lines (array3, argv[3]);*/
	/*if (rc != E_SUCCESS)*/
		/*{*/
			/*error (0, rc, "Failed to read file '%s'", argv[3]);*/
			/*goto Exit;*/
		/*}*/

	/*assert (rc == 0);*/

	/*if (array3->len == 0)*/
		/*{*/
			/*rc = set_error_code (D_LOCAL, E_NOLINE);*/
			/*error (0, rc, "Failed to read file '%s'", argv[3]);*/
			/*goto Exit;*/
		/*}*/

	/*assert (rc == 0);*/

	ptr_array_uniq (array1, cmpstringp);
	ptr_array_uniq (array2, cmpstringp);
	/*qsort (array3->pdata, array3->len, sizeof (char *), cmpstringp);*/

	rc = compare_arrays (comm, array1, array2, cmpstringp);
	if (rc != E_SUCCESS)
		{
			error (0, rc, "Error during comparing files '%s' and '%s'", argv[1], argv[2]);
			goto Exit;
		}

	printf ("Uniq lines to %s\n", argv[1]);
	for  (int i = 0;  i < comm->uniq_to_file1->len; i++)
		printf ("- %s\n", (char *) ptr_array_get (comm->uniq_to_file1, i));

	printf ("Uniq lines to %s\n", argv[2]);
	for  (int i = 0;  i < comm->uniq_to_file2->len; i++)
		printf ("- %s\n", (char *) ptr_array_get (comm->uniq_to_file2, i));

	printf ("Shared lines between %s and %s\n", argv[1], argv[2]);
	for  (int i = 0;  i < comm->shared->len; i++)
		printf ("- %s\n", (char *) ptr_array_get (comm->shared, i));

Exit:
	comm_free (comm, 0);
	ptr_array_free (array1, 1);
	ptr_array_free (array2, 1);
	/*ptr_array_free (array3, 1);*/
	return rc == E_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}
/*int*/
/*main (int argc, char *argv[])*/
/*{*/
	/*if (argc != 5)*/
		/*error (1, set_error_code (D_LOCAL, E_NOARG), "Usage %s N K n k", PACKAGE);*/

	/*int N = atoi (argv[1]);*/
	/*int K = atoi (argv[2]);*/
	/*int n = atoi (argv[3]);*/
	/*int k = atoi (argv[4]);*/

	/*hyper_s h;*/
	/*hyper (&h, N, K, n, k);*/

	/*printf ("[N=%d K=%d n=%d k=%d]\n", N, K, n, k);*/
	/*printf ("P(N eq %d) = %.*lf\n", k, N_DIGITS, h.pmf);*/
	/*printf ("P(N lt %d) = %.*lf\n", k, N_DIGITS, h.cdfe);*/
	/*printf ("P(N le %d) = %.*lf\n", k, N_DIGITS, h.cdfi);*/
	/*printf ("P(N gt %d) = %.*lf\n", k, N_DIGITS, 1 - h.cdfi);*/
	/*printf ("P(N ge %d) = %.*lf\n", k, N_DIGITS, 1 - h.cdfe);*/

	/*return 0;*/
/*}*/
