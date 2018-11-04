#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <error.h>
#include <comm.h>

Comm *
comm_new (DestroyNotify element_free_func, int *errnum)
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

	comm->uniq_to_file1 = ptr_array_new (element_free_func, errnum);
	if (comm->uniq_to_file1 == NULL)
		{
			assert (errnum == NULL || *errnum != 0);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	comm->uniq_to_file2 = ptr_array_new (element_free_func, errnum);
	if (comm->uniq_to_file2 == NULL)
		{
			assert (errnum == NULL || *errnum != 0);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

	comm->shared = ptr_array_new (element_free_func, errnum);
	if (comm->shared == NULL)
		{
			assert (errnum == NULL || *errnum != 0);
			goto Fail;
		}

	assert (errnum == NULL || *errnum == 0);

Exit:
	return comm;

Fail:
	comm_free (comm, 0);
	goto Exit;
}

void
comm_free (Comm *comm, int free_segment)
{
	if (comm == NULL)
		return;

	if (comm->uniq_to_file1 != NULL)
		ptr_array_free (comm->uniq_to_file1, free_segment);

	if (comm->uniq_to_file2 != NULL)
		ptr_array_free (comm->uniq_to_file2, free_segment);

	if (comm->shared != NULL)
		ptr_array_free (comm->shared, free_segment);

	free (comm);
}

int
compare_arrays (Comm *comm, PtrArray *array1, PtrArray *array2,
		CompareFunc compare_ptr)
{
	assert (comm != NULL);
	assert (compare_ptr != NULL);
	assert (array1 != NULL && array1->len > 0 && array1->pdata != NULL);
	assert (array2 != NULL && array2->len > 0 && array2->pdata != NULL);

	int errnum = 0;
	int i = 0, j = 0;
	errno = 0;

	while (i < array1->len)
		{
			void *ptr1 = ptr_array_get (array1, i);

			if (j < array2->len)
				{
					void *ptr2 = ptr_array_get (array2, j);
					int cmp = compare_ptr (&ptr1, &ptr2);

					if (!cmp)
						{
							errnum = ptr_array_add (comm->shared, ptr1);
							if (errnum != E_SUCCESS)
								goto Exit;
							i++, j++;
							continue;
						}
					else if (cmp > 0)
						{
							errnum = ptr_array_add (comm->uniq_to_file2, ptr2);
							if (errnum != E_SUCCESS)
								goto Exit;
							j++;
							continue;
						}
				}

			errnum = ptr_array_add (comm->uniq_to_file1, ptr1);
			if (errnum != E_SUCCESS)
				goto Exit;
			i++;
		}

	for (; j < array2->len; j++)
		{
			void *ptr2 = ptr_array_get (array2, j);

			errnum = ptr_array_add (comm->uniq_to_file2, ptr2);
			if (errnum != E_SUCCESS)
				goto Exit;
		}

Exit:
	return errnum;
}
