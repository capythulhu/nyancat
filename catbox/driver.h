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
#include "operations.h"

#define CACHE_SIZE 4
#define RESERVED_REGS 2

// Driver structure
typedef struct _driver {
    qubit       *qregs;
    char        *cregs;
    char        cache[CACHE_SIZE];
    unsigned    qtotal;
    unsigned    pointer;
} driver;

// Creates a new driver
driver *new_driver(unsigned qtotal) {
    driver *d = malloc(sizeof(driver));
    d->qtotal = qtotal;
    d->qregs = malloc(sizeof(qubit) * d->qtotal);
    d->cregs = calloc(sizeof(char), RESERVED_REGS);
    d->pointer = 0;
    int i;
    for(i = 0; i < CACHE_SIZE; i++) d->cache[i] = 0;
    for(i = 0; i < d->qtotal; i++) d->qregs[i] = ZERO;
    srand(time(NULL));
    return d;
}

// Checks if a qubit has been allocated
bool valid_qubit(driver *d, int addr) {
    return addr >= 0 && d && d->qregs != NULL;
}

// Gets a qubit from driver
qubit get_qubit(driver *d, int addr) {
    if(!valid_qubit(d, addr)) return (qubit){0, 0};
    return d->qregs[addr];
}

// Sets a qubit in driver
bool set_qubit(driver *d, int addr, qubit q) {
    if(!valid_qubit(d, addr)) return false;
    d->qregs[addr] = q;
    return true;
}

// Applies the Hadamard gate
bool apply_H(driver *d, int addr) {
    if(!valid_qubit(d, addr)) return false;
    qubit q = get_qubit(d, addr);
    show_percentages(q);
    set_qubit(d, addr, hadamard(q));
    return true;
}

// Applies the Pauli X gate
bool apply_X(driver *d, int addr) {
    if(!valid_qubit(d, addr)) return false;
    qubit q = get_qubit(d, addr);
    set_qubit(d, addr, pauli_x(q));
    return true;
}

// Applies the Pauli Y gate
bool apply_Y(driver *d, int addr) {
    if(!valid_qubit(d, addr)) return false;
    qubit q = get_qubit(d, addr);
    set_qubit(d, addr, pauli_y(q));
    return true;
}

// Applies the Pauli Z gate
bool apply_Z(driver *d, int addr) {
    if(!valid_qubit(d, addr)) return false;
    qubit q = get_qubit(d, addr);
    set_qubit(d, addr, pauli_z(q));
    return true;
}

// Applies the Measurement gate
int apply_M(driver *d, int addr) {
    if(!valid_qubit(d, addr)) return -1;
    qubit q = get_qubit(d, addr);
    // Jumps one number in the stream to avoid repetition
    rand();
    int bit = (double)rand() / RAND_MAX > pow(q.zero, 2);
    set_qubit(d, addr, bit ? ONE : ZERO);
    return bit;
}

// Process a command line
bool process_command(driver *d, command c) {
    if(!d) return false;
    switch(c.op) {
        case OP_END:
            d->cregs[3] = c.args[0];
            return false;
        case OP_CMP:
            d->cregs[1] = apply_M(d, c.args[0]);
            d->cregs[0] = d->cregs[1] >= 0 ? 
            d->cregs[1] > c.args[1] ? 1
            : d->cregs[1] < c.args[1] ? -1
            : 0 
            : -2;
            d->pointer++;
            return true;
        case OP_MOV:
            d->cache[c.args[1]] = d->cregs[c.args[0]];
            d->pointer++;
            return true;
        case OP_JE:
            if(d->cregs[0] == 0) d->pointer = c.args[1]; 
            else d->pointer++;
            return true;
        case OP_JNE:
            if(d->cregs[0] != 0) d->pointer = c.args[1]; 
            else d->pointer++;
            return true;
        case OP_JG:
            if(d->cregs[0] == 1) d->pointer = c.args[1]; 
            else d->pointer++;
            return true;
        case OP_JGE:
            if(d->cregs[0] == 0 || d->cregs[0] == 1) d->pointer = c.args[1]; 
            else d->pointer++;
            return true;
        case OP_JL:
            if(d->cregs[0] == -1) d->pointer = c.args[1]; 
            else d->pointer++;
            return true;
        case OP_JLE:
            if(d->cregs[0] == 0 || d->cregs[0] == -1) d->pointer = c.args[1]; 
            else d->pointer++;
            return true;
        case OP_M:
            d->cregs[0] = apply_M(d, c.args[0]);
            d->pointer++;
            return true;
        case OP_H:
            d->cregs[0] = apply_H(d, c.args[0]);
            d->pointer++;
            return true;
        case OP_X:
            d->cregs[0] = apply_X(d, c.args[0]);
            d->pointer++;
            return true;
        case OP_Y:
            d->cregs[0] = apply_Y(d, c.args[0]);
            d->pointer++;
            return true;
        case OP_Z:
            d->cregs[0] = apply_Z(d, c.args[0]);
            d->pointer++;
            return true;
    }
}

// Process an algorithm
void process_algorithm(driver *d, command *alg, bool echo) {
    if(!d || !alg) return;
    d->pointer = 0;
    int i = false;
    do {
        i = process_command(d, alg[d->pointer]);
    } while(i == true);
    if(echo) printf("Algorithm finished with return code %i.\n", d->cregs[3]);
    free(d->qregs);
    free(d->cregs);
}

#endif