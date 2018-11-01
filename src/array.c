#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <error.h>
#include <array.h>

struct _RealPtrArray
{
	void          **pdata;
	size_t          len;
	size_t          alloc;
	DestroyNotify   element_free_func;
};

typedef struct _RealPtrArray RealPtrArray;

PtrArray *
ptr_array_new (DestroyNotify element_free_func, int *errnum)
{
	assert (errnum == NULL || *errnum == 0);
	errno = 0;

	RealPtrArray *array = NULL;
	array = (RealPtrArray *) calloc (1, sizeof (RealPtrArray));

	if (array == NULL)
		{
			if (errnum != NULL)
				*errnum = set_error_code (D_SYSTEM, errno);
			return NULL;
		}

	assert (errnum == NULL || *errnum == 0);
	array->element_free_func = element_free_func;

	return (PtrArray *) array;
}

void **
ptr_array_free (PtrArray *array, int free_segment)
{
	if (array == NULL)
		return NULL;

	RealPtrArray *rarray = (RealPtrArray *) array;
	void **segment = NULL;

	if (free_segment)
		{
			void **data = rarray->pdata;
			rarray->pdata = NULL;

			if (rarray->element_free_func != NULL)
				{
					for (int i = 0; i < rarray->len; i++)
						rarray->element_free_func (data[i]);
				}

			free (data);
		}
	else
		segment = rarray->pdata;

	free (rarray);
	return segment;
}

static size_t
nearest_pow (size_t num)
{
	size_t n = 1;

	while (n < num && n > 0)
		n <<= 1;

	return n ? n : num;
}

static int
ptr_array_maybe_expand (RealPtrArray *array, int len)
{
	int errnum = 0;
	errno = 0;

	if ((array->len + len) > array->alloc)
		{
			size_t old_alloc = array->alloc;
			array->alloc = nearest_pow (array->len + len);

			array->pdata = realloc (array->pdata,
					sizeof (void *) * array->alloc);

			if (array->pdata == NULL)
				goto Fail;

			for ( ; old_alloc < array->alloc; old_alloc++)
				array->pdata [old_alloc] = NULL;
		}

Exit:
	return errnum;

Fail:
	errnum = set_error_code (D_SYSTEM, errno);
	goto Exit;
}

int
ptr_array_add (PtrArray *array, void *ptr)
{
	RealPtrArray *rarray = (RealPtrArray *) array;

	assert (rarray != NULL);
	assert (rarray->len == 0 || (rarray->len != 0 && rarray->pdata != NULL));

	int errnum = 0;
	errnum = ptr_array_maybe_expand (rarray, 1);

	if (errnum == E_SUCCESS)
		rarray->pdata [rarray->len++] = ptr;

	return errnum;
}
