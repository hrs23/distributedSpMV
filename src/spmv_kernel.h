#if defined(CPU) || defined(MIC)
#include <mkl.h>
#endif
#include "sparse_matrix.h"
#include "vector.h"
int SpMVInternal (const SparseMatrix & A, Vector & x, Vector & y);
int SpMVExternal (const SparseMatrix & A, Vector & x, Vector & y);
int SpMVDenseInternal (const SparseMatrix & A, Vector & x, Vector & y);
