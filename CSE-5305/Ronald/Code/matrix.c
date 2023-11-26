#include "matrix.h"
#include <time.h>

#define MODULUS 7

TMatrix * newMatrix(unsigned int nrow, unsigned int ncol)
{
    /* Allocate space for TMatrix */
    TMatrix * newM = malloc(sizeof(TMatrix));

    /* Allocate space for a rows X cols matrix
       allocate pointers to row arrays      */
    TElement ** data = malloc(nrow * sizeof(TElement *));

    /* allocate space for all elements in one call */
    int * storage = malloc (nrow*ncol * sizeof(TElement));

    if (newM == NULL || data == NULL || storage == NULL) {
        free(newM);
        free(data);
        free(storage);
        return NULL;
    }

    /* set row array pointers */
    for (size_t i = 0; i < nrow; i++) {
        data[i] = storage + i * ncol;
	}

    newM->nrow = nrow;
    newM->ncol = ncol;
    newM->data = data;
    return newM;
}

void freeMatrix(TMatrix * m)
{
    if (m == NULL)          /* remember to check in every function */
        return;
    if (m->data)  {
        free(m->data[0]);   /* free the storage allocated for data */
        free(m->data);      /* free the array of pointers to row pointers */
    }
    free(m);                /* free the matrix itself */
}

TMatrix * fillMatrix(TMatrix * m)
{
    static int first=1;

    if (m == NULL || m->data == NULL)
        return m;

    // again, for the purpose of this assignment.
    if (first) {
        srand(clock());
        first = 0;
    }

    unsigned int i, j;
    for (i = 0; i < m->nrow; i ++)
        for (j = 0; j < m->ncol; j ++)
            m->data[i][j] = (TElement) rand() % MODULUS;
    return m;
}

int compareMatrix(TMatrix * m, TMatrix *n)
{
    if (m == NULL || n == NULL)
        return -1;
    if (m->nrow != n->nrow || m->ncol != n->ncol)
        return -2;

    unsigned i, j;
    for (i = 0; i < m->nrow; i ++)
        for (j = 0; j < m->ncol; j ++)
            if ( m->data[i][j] != n->data[i][j] ) {
                printf("element[%u][%u]  %d  %d.\n", i, j, m->data[i][j], n->data[i][j]);
                return 1;
            }
    return 0;
}

TMatrix * mulMatrix(TMatrix *m, TMatrix *n)
{
    if (     m == NULL || n == NULL
          || m->ncol != n->nrow   )
        return NULL;

    TMatrix * t = newMatrix(m->nrow, n->ncol);
    if (t == NULL)
        return t;

    // zero out result matrix with one memset since rows are contiguously allocated
    memset( &(t->data[0][0]), 0, sizeof(TElement) * t->nrow * t->ncol);

    // Use IKJ order for best cache performance
    for(int i=0; i < m->nrow; i++)
        for(int k=0; k < m->ncol; k++)
            for(int j=0; j < n->ncol; j++)
                t->data[i][j] += m->data[i][k] * n->data[k][j];

    return t;
}