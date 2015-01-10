#include <mkl.h>
#include "sparse_matrix.h"
#include "vector.h"
int SpMVInternal (const SparseMatrix & A, Vector & x, Vector & y);
int SpMVExternal (const SparseMatrix & A, Vector & x, Vector & y);
