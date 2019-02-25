#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "error.h"
#include "comm.h"

static Comm *
comm_new (int *errnum)
{
	assert (errnum == NULL || *errnum == 0);

	Comm *comm = NULL;
	errno = 0;

	comm = (Comm *) calloc (1, sizeof (Comm));
	if (comm == NULL)
		{
			*errnum = set_error_code (D_SYSTEM, errno);
			goto Exit;
		}

	comm->uniq_to_file1 = ptr_array_new (NULL, errnum);
	if (comm->uniq_to_file1 == NULL)
		{
			assert (errnum == NULL || *errnum != 0);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	comm->uniq_to_file2 = ptr_array_new (NULL, errnum);
	if (comm->uniq_to_file2 == NULL)
		{
			assert (errnum == NULL || *errnum != 0);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	comm->shared = ptr_array_new (NULL, errnum);
	if (comm->shared == NULL)
		{
			assert (errnum == NULL || *errnum != 0);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

Exit:
	return comm;

Fail:
	comm_free (comm);
	comm = NULL;
	goto Exit;
}

void
comm_free (Comm *comm)
{
	if (comm == NULL)
		return;

	if (comm->uniq_to_file1 != NULL)
		ptr_array_free (comm->uniq_to_file1, 1);

	if (comm->uniq_to_file2 != NULL)
		ptr_array_free (comm->uniq_to_file2, 1);

	if (comm->shared != NULL)
		ptr_array_free (comm->shared, 1);

	free (comm);
}

Comm *
compare_arrays (PtrArray *array1, PtrArray *array2,
		CompareFunc compare_ptr, int *errnum)
{
	assert (compare_ptr != NULL);
	assert (array1 != NULL && array1->len > 0 && array1->pdata != NULL);
	assert (array2 != NULL && array2->len > 0 && array2->pdata != NULL);
	assert (errnum == NULL || *errnum == 0);

	Comm *comm = NULL;
	int i = 0, j = 0;
	int tmp_errnum = 0;
	errno = 0;

	comm = comm_new (errnum);
	if (comm == NULL)
		{
			assert (errnum == NULL || *errnum != 0);
			goto Exit;
		}

	assert (errnum == NULL || *errnum == 0);

	while (i < array1->len)
		{
			void *ptr1 = ptr_array_get (array1, i);

			if (j < array2->len)
				{
					void *ptr2 = ptr_array_get (array2, j);
					int cmp = compare_ptr (&ptr1, &ptr2);

					if (!cmp)
						{
							tmp_errnum = ptr_array_add (comm->shared, ptr1);
							if (tmp_errnum != E_SUCCESS)
								{
									if (errnum != NULL)
										*errnum = tmp_errnum;
									goto Fail;
								}
							i++, j++;
							continue;
						}
					else if (cmp > 0)
						{
							tmp_errnum = ptr_array_add (comm->uniq_to_file2, ptr2);
							if (tmp_errnum != E_SUCCESS)
								{
									if (errnum != NULL)
										*errnum = tmp_errnum;
									goto Fail;
								}
							j++;
							continue;
						}
				}

			tmp_errnum = ptr_array_add (comm->uniq_to_file1, ptr1);
			if (tmp_errnum != E_SUCCESS)
				{
					if (errnum != NULL)
						*errnum = tmp_errnum;
					goto Fail;
				}
			i++;
		}

	for (; j < array2->len; j++)
		{
			void *ptr2 = ptr_array_get (array2, j);

			tmp_errnum = ptr_array_add (comm->uniq_to_file2, ptr2);
			if (tmp_errnum != E_SUCCESS)
				{
					if (errnum != NULL)
						*errnum = tmp_errnum;
					goto Fail;
				}
		}

Exit:
	return comm;

Fail:
	comm_free (comm);
	comm = NULL;
	goto Exit;
}
