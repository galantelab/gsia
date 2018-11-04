#ifndef COMM_H
#define COMM_H

#include "array.h"
#include "types.h"

struct _Comm
{
	PtrArray *uniq_to_file1;
	PtrArray *uniq_to_file2;
	PtrArray *shared;
};

typedef struct _Comm Comm;

void comm_free (Comm *comm, int free_segment);
Comm *comm_new (DestroyNotify element_free_func, int *errnum);
int compare_arrays (Comm *comm, PtrArray *array1, PtrArray *array2, CompareFunc compare_ptr);

#endif /* comm.h */
