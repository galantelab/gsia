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

Comm * compare_arrays (PtrArray *array1, PtrArray *array2, CompareFunc compare_ptr, int *errnum);
void   comm_free      (Comm *comm);

#endif /* comm.h */
