#ifndef QUBIT_H
#define QUBIT_H

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDBOOL_H
#define STDBOOL_H
#include <stdbool.h>
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

// Checks if qubit's eigenvalues are 100% in total
bool valid_qubit(qubit q) {
    double sum = pow(q.zero, 2) + pow(q.one, 2);
    return sum >= 1 - PROBABILITY_ERROR && sum <= 1 + PROBABILITY_ERROR;
}

// Transforms the qubit into a matrix with it's eigenvalues
matrix *qubit_to_matrix(qubit q) {
    matrix *m = new_matrix(2, 1);
    populate_matrix(m,  q.zero,
                        q.one);
    return m;
}

// Transforms a matrix into a qubit's eigenvalues
qubit matrix_to_qubit(matrix *m) {
    qubit q = { get_matrix_val(m, 0, 0),
                get_matrix_val(m, 1, 0)};
    return q;
}

// Prints qubit's eigenvalues
char *show_qubit(qubit q) {
    char *a = malloc(sizeof(char) * (1<<8));
    sprintf(a, "(%lf, %lf)", q.zero, q.one);
    return a;
}

// Prints qubits eigenstates chances
char *show_percentages(qubit q) {
    char *a = malloc(sizeof(char) * (1<<8));
    sprintf(a, "|0> = %.2lf%%, |1> = %.2lf%%", q.zero * q.zero * 100, q.one * q.one * 100);
    return a;
}

// Applies the Hadamard gate
qubit hadamard(qubit q) {
    matrix *a = new_matrix(2, 2);
    populate_matrix(a,  (double)1/sqrt(2), (double)1/sqrt(2),
                        (double)1/sqrt(2), (double)-1/sqrt(2));
    matrix *b = qubit_to_matrix(q);
    matrix *c = multiply_matrix(a, b);
    q = matrix_to_qubit(c);
    free(a);
    free(b);
    free(c);
    return q;
}

// Applies the Pauli X gate
qubit pauli_x(qubit q) {
    matrix *a = new_matrix(2, 2);
    populate_matrix(a,  (double)0, (double)1,
                        (double)1, (double)0);
    matrix *b = qubit_to_matrix(q);
    matrix *c = multiply_matrix(a, b);
    q = matrix_to_qubit(c);
    free(a);
    free(b);
    free(c);
    return q;
}

// Applies the Pauli Y gate
qubit pauli_y(qubit q) {
    matrix *a = new_matrix(2, 2);
    populate_matrix(a,  (double)0, (double)1,
                        (double)1, (double)0);
    matrix *b = qubit_to_matrix(q);
    matrix *c = multiply_matrix(a, b);
    q = matrix_to_qubit(c);
    free(a);
    free(b);
    free(c);
    return q;
}

// Applies the Pauli Z gate
qubit pauli_z(qubit q) {
    matrix *a = new_matrix(2, 2);
    populate_matrix(a,  (double)1, (double)0,
                        (double)0, (double)-1);
    matrix *b = qubit_to_matrix(q);
    matrix *c = multiply_matrix(a, b);
    q = matrix_to_qubit(c);
    free(a);
    free(b);
    free(c);
    return q;
}

#endif