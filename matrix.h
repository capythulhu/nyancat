#ifndef MATRIX_H
#define MATRIX_H

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

// Matrix structure
typedef struct _matrix {
    double *vals;
    int rows, columns;
} matrix;

// Allocates a new matrix
matrix *new_matrix(int rows, int columns) {
    matrix *m = (matrix*)malloc(sizeof(matrix));
    m->rows = rows;
    m->columns = columns;
    m->vals = (double*)calloc(sizeof(double), rows * columns);
    return m;
}

// Gets value from matrix
double get_matrix_val(matrix *m, int row, int column) {
    if(!m || row >= m->rows || row < 0 
        || column >= m->columns || column < 0) return 0;
    return m->vals[row * m->rows + column];
}

// Sets value on matrix
int set_matrix_val(matrix *m, int row, int column, double val) {
    if(!m || row >= m->rows || row < 0 
        || column >= m->columns || column < 0) return 0;
    m->vals[row * m->rows + column] = val;
    return 1;
}

// Frees matrix
int free_matrix(matrix *m) {
    if(!m) return 0;
    free(m->vals);
    free(m);
    m = NULL;
    return 1;
}

#endif