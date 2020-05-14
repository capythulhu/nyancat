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
#include "complex_num.h"

#define PROBABILITY_ERROR 1e-7

// Qubit structure
typedef struct _qubit {
    complexnum zero;
    complexnum one;
} qubit;

#define ZERO (qubit){real(1), real(0)}
#define ONE (qubit){real(0), real(1)}

// Checks if qubit's eigenvalues are 100% in total
bool valid_qubit(qubit q) {
    double sum = pow_complex(q.zero, 2).r + pow_complex(q.one, 2).r;
    //printf("asd: %f\n", sum);
    return fabs(sum) >= 1 - PROBABILITY_ERROR && sum <= 1 + PROBABILITY_ERROR;
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
                get_matrix_val(m, 1, 0) };
    return q;
}

// Prints qubit's eigenvalues
char *show_qubit(qubit q) {
    char *a = malloc(sizeof(char) * ((1<<5) + 4));
    sprintf(a, "(%s, %s)", show_complex(q.zero), show_complex(q.one));
    return a;
}

// Prints qubits eigenstates chances
char *show_percentages(qubit q) {
    char *a = malloc(sizeof(char) * (1<<8));
    sprintf(a, "|0> = %.2lf%%, |1> = %.2lf%%", q.zero.r * q.zero.r * 100, q.one.r * q.one.r * 100);
    return a;
}

// Applies the Hadamard gate
qubit hadamard(qubit q) {
    matrix *a = new_matrix(2, 2);
    populate_matrix(a,  real(1/sqrt(2)), real(1/sqrt(2)),
                        real(1/sqrt(2)), real(-1/sqrt(2)));
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
    populate_matrix(a,  real(0), real(1),
                        real(1), real(0));
    print_matrix(a);
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
    populate_matrix(a,  real(0), imag(-1),
                        imag(1), real(0));
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
    populate_matrix(a,  real(1), real(0),
                        real(0), real(-1));
    matrix *b = qubit_to_matrix(q);
    matrix *c = multiply_matrix(a, b);
    q = matrix_to_qubit(c);
    free(a);
    free(b);
    free(c);
    return q;
}

#endif