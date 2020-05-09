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

#ifndef STDBOOL_H
#define STDBOOL_H
#include <stdbool.h>
#endif

#ifndef LIMITS_H
#define LIMITS_H
#include <limits.h>
#endif

#ifndef TIME_H
#define TIME_H
#include <time.h>
#endif

#ifndef MATH_H
#define MATH_H
#include <math.h>
#endif

#include "qubit.h"

// Driver structure
typedef struct _driver {
    qubit **mem;
    unsigned capacity;
    unsigned used;
} driver;

// Creates a new driver
driver *new_driver(int capacity) {
    driver *d = malloc(sizeof(driver));
    d->mem = calloc(sizeof(qubit*), capacity);
    d->capacity = capacity;
    d->used = 0;
    srand(time(NULL));
    return d;
}

// Gets a qubit from a driver
qubit *get_qubit(driver *d, int addr) {
    if(!d || addr >= d->capacity || addr < 0) return NULL;
    return d->mem[addr];
}

// Sets a qubit of a driver
bool set_qubit(driver *d, int addr, qubit q) {
    if(!is_qubit_valid || !d || addr >= d->capacity || addr < 0) return false;
    *(d->mem[addr]) = q;
    return true;
}

// Allocates a qubit in driver
int allocate_qubit(driver *d) {
    if(d->used >= d->capacity) return -1;
    int i = 0;
    while(d->mem[i] && i < d->capacity) i++;
    if(i >= d->capacity) return -1;
    d->mem[i] = malloc(sizeof(qubit));
    *d->mem[i] = ZERO;
    return i;
}

// Frees a qubit in driver
bool free_qubit(driver *d, int addr) {
    if(d->used <= 0) return false;
    free(d->mem[addr]);
    d->mem[addr] = NULL;
    return true;
}

// Prints qubits eigenvalues
char *show_qubit(driver *d, int addr) {
    qubit q = *get_qubit(d, addr);
    char *a = malloc(sizeof(char) * (1<<8));
    sprintf(a, "(%lf, %lf)", q.zero, q.one);
    return a;
}

// Prints qubits eigenstates chances
char *show_percentages(driver *d, int addr) {
    qubit q = *get_qubit(d, addr);
    char *a = malloc(sizeof(char) * (1<<8));
    sprintf(a, "|0> = %.2lf%%, |1> = %.2lf%%", q.zero * q.zero * 100, q.one * q.one * 100);
    return a;
}

// Applies the Hadamard gate
int H(driver *d, int addr) {
    qubit *q = get_qubit(d, addr);
    if(!is_qubit_valid(*q)) return 0;
    matrix *a = new_matrix(2, 2);
    set_matrix_val(a, 0, 0, 1/sqrt(2));
    set_matrix_val(a, 0, 1, 1/sqrt(2));
    set_matrix_val(a, 1, 0, 1/sqrt(2));
    set_matrix_val(a, 1, 1, -1/sqrt(2));
    matrix *b = qubit_to_matrix(*q);
    matrix *c = multiply_matrix(a, b);
    q->zero = get_matrix_val(c, 0, 0);
    q->one = get_matrix_val(c, 1, 0);
    free(a);
    free(b);
    free(c);
}

// Applies the Measurement gate
int M(driver *d, int addr) {
    qubit *q = get_qubit(d, addr);
    if(!is_qubit_valid(*q)) return -1;
    // Jumps one number in the stream to avoid repetition
    rand();
    int bit = (double)rand() / RAND_MAX > pow(q->zero, 2);
    set_qubit(d, addr, bit ? ONE : ZERO);
    return bit;
}

#endif