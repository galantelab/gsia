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
#include "gsia.h"

#define N_DIGITS 20
#define COMM_FILE "gsia_comm.txt"

struct _Gsia
{
	size_t       featurec;
	const char  *universe_file;
	const char  *list_file;
	const char **feature_file;
	PtrArray    *universe;
	PtrArray    *list;
	PtrArray   **feature;
	Comm        *universe_list;
	Comm       **universe_feature;
	Comm       **list_feature;
	Hyper      **stat;
};

typedef struct _Gsia Gsia;

static int
cmpstringp (const void *p1, const void *p2)
{
	return strcasecmp (* (char * const *) p1, * (char * const *) p2);
}

static void
gsia_free (Gsia *g)
{
	if (g == NULL)
		return;

	free ((char *) g->list_file);
	free ((char *) g->universe_file);
	ptr_array_free (g->list, 1);
	ptr_array_free (g->universe, 1);
	comm_free (g->universe_list);

	for (int i = 0; i < g->featurec; i++)
		{
			free ((char *) g->feature_file[i]);
			ptr_array_free (g->feature[i], 1);
			comm_free (g->universe_feature[i]);
			comm_free (g->list_feature[i]);
			hyper_free (g->stat[i]);
		}

	free (g->feature_file);
	free (g->feature);
	free (g->universe_feature);
	free (g->list_feature);
	free (g->stat);
	free (g);
}

static Gsia *
gsia_new (const char *list, const char *universe, size_t featurec,
		const char *feature[], int *errnum)
{
	assert (featurec > 0);
	assert (list != NULL && universe != NULL);
	assert (feature != NULL && *feature != NULL);
	assert (errnum == NULL || *errnum == 0);

	errno = 0;
	Gsia *g = NULL;

	g = calloc (1, sizeof (Gsia));
	if (g == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	g->featurec = featurec;

	g->list_file = strdup (list);
	if (g->list_file == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	g->universe_file = strdup (universe);
	if (g->universe_file == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	g->feature_file = calloc (featurec, sizeof (char **));
	if (g->feature_file == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	g->list = ptr_array_new (free, errnum);
	if (g->list == NULL)
		{
			assert (errnum == NULL || *errnum != 0);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	g->universe = ptr_array_new (free, errnum);
	if (g->universe == NULL)
		{
			assert (errnum == NULL || *errnum != 0);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	g->feature = calloc (featurec, sizeof (PtrArray **));
	if (g->feature == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	for (int i = 0; i < featurec; i++)
		{
			g->feature_file[i] = strdup (feature[i]);
			if (g->feature_file[i] == NULL)
				{
					if (errnum != NULL)
						*errnum = set_error_code (D_SYSTEM, errno);
					goto Fail;
				}

			assert (errnum == NULL || *errnum == 0);

			g->feature[i] = ptr_array_new (free, errnum);
			if (g->feature[i] == NULL)
				{
					assert (errnum == NULL || *errnum != 0);
					goto Fail;
				}

			assert (errnum == NULL || *errnum == 0);
		}

	g->universe_feature = calloc (featurec, sizeof (Comm **));
	if (g->universe_feature == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	g->list_feature = calloc (featurec, sizeof (Comm **));
	if (g->list_feature == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	g->stat = calloc (featurec, sizeof (Hyper **));
	if (g->stat == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

Exit:
	return g;

Fail:
	gsia_free (g);
	g = NULL;
	goto Exit;
}

static int
gsia_read (Gsia *g)
{
	assert (g != NULL);

	int errnum = 0;

	errnum = read_file_lines (g->list, g->list_file);
	if (errnum != E_SUCCESS)
		{
			error (0, 0, "Failed to read file '%s'", g->list_file);
			goto Exit;
		}

	if (g->list->len == 0)
		{
			errnum = set_error_code (D_LOCAL, E_NOLINE);
			error (0, 0, "Failed to read file '%s'", g->list_file);
			goto Exit;
		}

	errnum = read_file_lines (g->universe, g->universe_file);
	if (errnum != E_SUCCESS)
		{
			error (0, 0, "Failed to read file '%s'", g->universe_file);
			goto Exit;
		}

	if (g->universe->len == 0)
		{
			errnum = set_error_code (D_LOCAL, E_NOLINE);
			error (0, 0, "Failed to read file '%s'", g->universe_file);
			goto Exit;
		}

	for (int i = 0; i < g->featurec; i++)
		{
			errnum = read_file_lines (g->feature[i], g->feature_file[i]);
			if (errnum != E_SUCCESS)
				{
					error (0, 0, "Failed to read file '%s'", g->feature_file[i]);
					goto Exit;
				}

			if (g->feature[i]->len == 0)
				{
					errnum = set_error_code (D_LOCAL, E_NOLINE);
					error (0, 0, "Failed to read file '%s'", g->feature_file[i]);
					goto Exit;
				}
		}

Exit:
	return errnum;
}

static int
gsia_compare (Gsia *g)
{
	assert (g != NULL);

	int errnum = 0;

	ptr_array_uniq (g->list, cmpstringp);
	ptr_array_uniq (g->universe, cmpstringp);

	g->universe_list = compare_arrays (g->universe, g->list,
			cmpstringp, &errnum);
	if (errnum != E_SUCCESS)
		{
			assert (errnum != 0 && g->universe_list == NULL);
			error (0, 0, "Error during comparing files '%s' and '%s'",
					g->universe_file, g->list_file);
			goto Exit;
		}

	assert (g->universe_list != NULL);

	if (g->universe_list->shared->len == 0)
		{
			errnum = set_error_code (D_LOCAL, E_NOSHRED);
			error (0, 0, "Error during comparing files '%s' and '%s'",
					g->universe_file, g->list_file);
			goto Exit;
		}

	assert (errnum == 0);

	for (int i = 0; i < g->featurec; i++)
		{
			ptr_array_uniq (g->feature[i], cmpstringp);

			g->universe_feature[i] = compare_arrays (g->universe, g->feature[i],
					cmpstringp, &errnum);
			if (errnum != E_SUCCESS)
				{
					assert (g->universe_feature[i] == NULL);
					error (0, 0, "Error during comparing files '%s' and '%s'",
							g->universe_file, g->feature_file[i]);
					goto Exit;
				}

			assert (g->universe_feature[i] != NULL);

			if (g->universe_feature[i]->shared->len > 0)
				{
					g->list_feature[i] = compare_arrays (g->universe_list->shared,
							g->universe_feature[i]->shared, cmpstringp, &errnum);
					if (errnum != E_SUCCESS)
						{
							assert (g->list_feature[i] == NULL);
							error (0, 0, "Error during comparing files '%s' and '%s'",
									g->list_file, g->feature_file[i]);
							goto Exit;
						}

					assert (g->list_feature[i] != NULL);
				}
		}

Exit:
	return errnum;
}

static int
gsia_stat (Gsia *g)
{
	assert (g != NULL);

	int errnum = 0;
	int N = 0;
	int K = 0;
	int n = 0;
	int k = 0;

	for (int i = 0; i < g->featurec; i++)
		{
			if (g->list_feature[i] != NULL)
				{
					N = g->universe->len;
					K = g->universe_feature[i]->shared->len;
					n = g->universe_list->shared->len;
					k = g->list_feature[i]->shared->len;

					if (K <= N && n <= N && k <= n)
						{
							g->stat[i] = hyper (N, K, n, k, &errnum);
							if (errnum != E_SUCCESS)
								{
									assert (g->stat[i] == NULL);
									goto Exit;
								}

							assert (g->stat[i] != NULL);
						}
				}
		}

Exit:
	return errnum;
}

static void
gsia_print (const Gsia *g)
{
	assert (g != NULL);

	printf ("# name: metadata.tsv\n");
	printf ("Name\tVersion\n");
	printf ("%s\t%s\n", PACKAGE_NAME, PACKAGE_VERSION);

	printf ("\n");

	printf ("# name: universe.tsv\n");
	printf ("File\tTotal\n");
	printf ("%s\t%zu\n", g->universe_file, g->universe->len);

	printf ("\n");

	printf ("# name: list.tsv\n");
	printf ("File\tTotal\tSharedWithUniverse\t%%SharedWithUniverse\n");
	printf ("%s\t%zu\t%zu\t%.2f\n", g->list_file, g->list->len,
			g->universe_list->shared->len,
			(float) g->universe_list->shared->len * 100 / g->list->len);

	printf ("\n");

	printf ("# name: feature.tsv\n");
	printf (
			"FeatureID\tFile\tTotal\tSharedWithUniverse\t%%SharedWithUniverse\t"
			"SharedWithUniverseAndList\t%%SharedWithUniverseAndList\t"
			"P(X=x)\tP(X<x)\tP(X<=x)\tP(X>x)\tP(X>=x)\n"
	);

	for (int i = 0; i < g->featurec; i++)
		{
			printf ("%d\t%s\t%zu\t%zu\t%.2f",
					i + 1, g->feature_file[i], g->feature[i]->len,
					g->universe_feature[i]->shared->len,
					(float) g->universe_feature[i]->shared->len * 100 / g->feature[i]->len);

			if (g->list_feature[i] == NULL)
				{
					assert (g->universe_feature[i]->shared->len == 0);
					printf ("\t.\t.\t.\t.\t.\t.\t.\n");
					continue;
				}

			printf ("\t%zu\t%.2f",
					g->list_feature[i]->shared->len,
					(float) g->list_feature[i]->shared->len * 100 / g->feature[i]->len);

			if (g->stat != NULL)
				{
					printf ("\t%.*lf", N_DIGITS, g->stat[i]->pmf);
					printf ("\t%.*lf", N_DIGITS, g->stat[i]->cdfe_P);
					printf ("\t%.*lf", N_DIGITS, g->stat[i]->cdfi_P);
					printf ("\t%.*lf", N_DIGITS, g->stat[i]->cdfe_Q);
					printf ("\t%.*lf", N_DIGITS, g->stat[i]->cdfi_Q);
				}
			else
				{
					printf ("\t.\t.\t.\t.\t.");
				}

			printf ("\n");
		}
}

static int
gsia_save_comm (const Gsia *g)
{
	assert (g != NULL);

	FILE *fp = NULL;
	int i = 0;
	int j = 0;
	int errnum = 0;
	errno = 0;

	fp = fopen (COMM_FILE, "w");
	if (fp == NULL)
		goto Fail;

	fprintf (fp, "# name: list_shared_with_universe.txt\n");
	for (i = 0; i < g->universe_list->shared->len; i++)
		fprintf (fp, "%s\n", ptr_array_get (g->universe_list->shared, i));

	fprintf (fp, "\n");

	fprintf (fp, "# name: list_uniq.txt\n");
	for (i = 0; i < g->universe_list->uniq_to_file2->len; i++)
		fprintf (fp, "%s\n", ptr_array_get (g->universe_list->uniq_to_file2, i));

	for (j = 0; j < g->featurec; j++)
		{
			fprintf (fp, "\n");

			fprintf (fp, "# name: feature%d_shared_with_universe_and_list.txt\n", j + 1);
			if (g->list_feature[j] != NULL)
				{
					for (i = 0; i < g->list_feature[j]->shared->len; i++)
						fprintf (fp, "%s\n", ptr_array_get (g->list_feature[j]->shared, i));
				}

			fprintf (fp, "\n");

			fprintf (fp, "# name: feature%d_shared_with_universe.txt\n", j + 1);
			for (i = 0; i < g->universe_feature[j]->shared->len; i++)
				fprintf (fp, "%s\n", ptr_array_get (g->universe_feature[j]->shared, i));

			fprintf (fp, "\n");

			fprintf (fp, "# name: feature%d_uniq.txt\n", j + 1);
			for (i = 0; i < g->universe_feature[j]->uniq_to_file2->len; i++)
				fprintf (fp, "%s\n", ptr_array_get (g->universe_feature[j]->uniq_to_file2, i));
		}

	if (fclose (fp) == EOF)
		goto Fail;

Exit:
	return errnum;

Fail:
	errnum = set_error_code (D_SYSTEM, errno);
	error (0, 0, "Failed to write to '%s'", COMM_FILE);
	goto Exit;
}

int
gsia (const char *list, const char *universe,
		size_t featurec, const char *feature[])
{
	assert (list != NULL && universe != NULL);
	assert (feature != NULL && *feature != NULL);
	assert (featurec > 0);

	int errnum = 0;

	Gsia *g = gsia_new (list, universe,
			featurec, feature, &errnum);
	if (g == NULL)
		{
			assert (errnum != 0);
			goto Exit;
		}

	assert (errnum == 0);

	errnum = gsia_read (g);
	if (errnum != E_SUCCESS)
		goto Exit;

	errnum = gsia_compare (g);
	if (errnum != E_SUCCESS)
		goto Exit;

	errnum = gsia_stat (g);
	if (errnum != E_SUCCESS)
		goto Exit;

	gsia_print (g);

	errnum = gsia_save_comm (g);
	if (errnum != E_SUCCESS)
		goto Exit;

Exit:
	gsia_free (g);
	return errnum;
}
