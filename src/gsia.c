#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "hyper.h"
#include "array.h"
#include "comm.h"
#include "io.h"
#include "error.h"

#define N_DIGITS 20

struct _Gsia
{
	PtrArray *universe;
	PtrArray *list;
	PtrArray *feature[];
	size_t featurec;
};

typedef struct _Gsia Gsia;

static int
cmpstringp (const void *p1, const void *p2)
{
	return strcasecmp (* (char * const *) p1, * (char * const *) p2);
}

static Gsia *
gsia_new (size_t featurec, int *errnum)
{
	assert (featurec > 0);
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

	g->feature = calloc (featurec, sizeof (PtrArray *));
	if (g->feature == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			goto Fail;
		}

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

	for (int i = 0; i < featurec; i++)
		{
			g->feature[i] = ptr_array_new (free, errnum);
			if (g->feature[i] == NULL)
				{
					assert (errnum == NULL || *errnum != 0);
					goto Fail;
				}

			assert (errnum == NULL || *errnum == 0);
		}

Exit:
	return g;

Fail:
	gsia_free (g);
	g = NULL;
	goto Exit;
}

static void
gsia_free (Gsia *g)
{
	if (g == NULL)
		return;

	ptr_array_free (g->list, 1);
	ptr_array_free (g->universe, 1);

	for (int i = 0; i < g->featurec; i++)
		ptr_array_free (g->feature[i], 1);

	free (g->feature);
	free (g);
}

static int
gsia_read (Gsia *g, const char *list, const char *universe,
		size_t featurec, va_list *ap)
{
	asssert (g != NULL);
	assert (list != NULL && universe != NULL && ap != NULL);
	assert (featurec > 0);

	int errnum = 0

	errnum = read_file_lines (g->list, list);
	if (errnum != E_SUCCESS)
		goto Exit;

	errnum = read_file_lines (g->universe, universe);
	if (errnum != E_SUCCESS)
		goto Exit;

	for (int i = 0; i < featurec; i++)
		{
			const char *feature = va_arg (*ap, const char *);
			errnum = read_file_lines (g->feature[i], feature);
			if (errnum != E_SUCCESS)
				goto Exit;
		}

Exit:
	return errnum;
}

int
gsia (const char *list, const char *universe,
		size_t featurec, ...)
{
	assert (list != NULL && universe != NULL);
	assert (featurec > 0);

	va_list ap;
	va_start (ap, featurec);

	int errnum = 0;

	Gsia *g = gsia_new (featurec, &errnum);
	if (g == NULL)
		{
			assert (errnum != 0);
			goto Exit;
		}

	assert (errnum == 0);

	errnum = gsia_read (g, list, universe, featurec, &ap);
	if (errnum != E_SUCCESS)
		goto Exit;

Exit:
	va_end (ap);
	gsia_free (g);
	return errnum;
}
