#ifndef MATRIX_H
#define MATRIX_H

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
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
    return m->vals[row * m->columns + column];
}

// Sets value on matrix
int set_matrix_val(matrix *m, int row, int column, double val) {
    if(!m || row >= m->rows || row < 0 
        || column >= m->columns || column < 0) return 0;
    m->vals[row * m->columns + column] = val;
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

matrix *multiply_matrix(matrix *m, matrix *n) {
    if(m->columns != n->rows) return NULL;
    matrix *r = new_matrix(m->rows, n->columns);
    double val;
    int i, j, k;
    for(i = 0; i < r->rows; i++) {
        for(j = 0; j < r->columns; j++) {
            val = 0;
            for(k = 0; k < m->columns; k++) {
                val += get_matrix_val(m, i, k) * get_matrix_val(n, k, j);
            }
            set_matrix_val(r, i, j, val);
        }
    }
    return r;
}

void print_matrix(matrix *m) {
    int i, j;
    for(i = 0; i < m->rows; i++) {
        printf("| ");
        for(j = 0; j < m->columns; j++) {
            printf("%f ", get_matrix_val(m, i, j));
        }
        printf("|\n");
    }
}

#endif