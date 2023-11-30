#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int TElement;

typedef struct Matrix {
    unsigned int nrow;
    unsigned int ncol;
    TElement **data;
} TMatrix;

#endif