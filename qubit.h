#ifndef QUBIT_H
#define QUBIT_H

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef MATH_H
#define MATH_H
#include <math.h>
#endif

#include "matrix.h"

#define PROBABILITY_ERROR 1e-7

// Qubit structure
typedef struct _qubit {
    double zero;
    double one;
} qubit;

#define ZERO (qubit){1.0F, 0.0F}
#define ONE (qubit){0.0F, 1.0F}

// Checks if the probabilities of the qubit result in 100%
int is_qubit_valid(qubit q) {
    double sum = (q.zero * q.zero) + (q.one * q.one);
    return sum <= 1.0F + PROBABILITY_ERROR || sum >= 1.0F - PROBABILITY_ERROR;
}

// Transforms the qubit into a matrix with it's eigenvalues
matrix *qubit_to_matrix(qubit q) {
    matrix *m = new_matrix(2, 1);
    set_matrix_val(m, 0, 0, q.zero);
    set_matrix_val(m, 1, 0, q.one);
    return m;
}

#endif