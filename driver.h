#ifndef DRIVER_H
#define DRIVER_H

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef MATH_H
#define MATH_H
#include <math.h>
#endif

// Qubit structure
typedef struct _qubit {
    double zero;
    double one;
} qubit;

// Checks if all probabilities results in 100%
static int is_probability_valid(double zero, double one) {
    return zero + one == 1.0F;
}

// Checks if the probabilities of the qubit results in 100%
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

}

#endif