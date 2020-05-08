#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef MATH_H
#define MATH_H
#include <math.h>
#endif

#include "matrix.h"

// Qubit structure
typedef struct _qubit {
    double zero;
    double one;
} qubit;

// Checks if all probabilities result in 100%
static int is_probability_valid(double a, double b) {
    return (a * a) + (b * b) == 1.0F;
}

// Checks if the probabilities of the qubit result in 100%
static int is_qubit_valid(qubit *q) {
    return is_probability_valid(q->zero, q->one);
}

// Allocates a new qubit
qubit *new_qubit(double zero, double one) {
    if(!is_probability_valid(zero, one)) return NULL;
    qubit *q = (qubit*)malloc(sizeof(qubit));
    q->zero = zero;
    q->one = one;
}

// Frees a qubit
int free_qubit(qubit *q) {
    if(!q) return 0;
    free(q);
    q = NULL;
    return 1;
}

// Applies the Hadamard gate
int H(qubit *q) {
    if(!is_qubit_valid(q)) return 0;
    matrix *a = new_matrix(2, 2);
    set_matrix_val(a, 0, 0, 1/sqrt(2));
    set_matrix_val(a, 0, 1, 1/sqrt(2));
    set_matrix_val(a, 1, 0, 1/sqrt(2));
    set_matrix_val(a, 1, 1, -1/sqrt(2));
    matrix *b = new_matrix(2, 1);
    set_matrix_val(b, 0, 0, q->zero);
    set_matrix_val(b, 1, 0, q->one);
    matrix *c = multiply_matrix(a, b);
    q->zero = get_matrix_val(c, 0, 0);
    q->one = get_matrix_val(c, 1, 0);
    free(a);
    free(b);
    free(c);
}