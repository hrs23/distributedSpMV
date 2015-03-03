#pragma once
#include "sparse_matrix.h"
#include "vector.h"
int SpMV (const SparseMatrix &A, Vector &x, Vector &y);
int SpMV_overlap (const SparseMatrix &A, Vector &x, Vector &y);
int SpMV_measurement_once (const SparseMatrix &A, Vector &x, Vector &y);
