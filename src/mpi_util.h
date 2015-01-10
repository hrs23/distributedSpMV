#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <mpi.h>
#include "sparse_matrix.h"
#include "vector.h"
using namespace std;

void PrintHostName ();
void LoadInput (const string &partFile, SparseMatrix &A, Vector &x);
void CreateZeroVector (Vector &x, int length);
void PrintResult (SparseMatrix &A, Vector &y);

void DeleteSparseMatrix (SparseMatrix & A);
void DeleteVector (Vector & x);


