#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include "types.h"

struct _PtrArray
{
	void   **pdata;
	size_t   len;
};

typedef struct _PtrArray PtrArray;

#define ptr_array_get(array, index) ((array)->pdata)[index]

PtrArray *ptr_array_new (DestroyNotify element_free_func, int *errnum);

void **ptr_array_free (PtrArray *array, int free_segment);

int ptr_array_add (PtrArray *array, void *ptr);

#endif /* array.h */
