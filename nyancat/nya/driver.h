#ifndef NYANCAT_NYA_DRIVER_H
#define NYANCAT_NYA_DRIVER_H

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

#include "../math/qubit.h"
#include "../utils/list.h"
#include "errors.h"
#include "operations.h"
#include "lexer.h"

#define RESERVED_REGS 2
#define REG(d, addr) (d->ctotal + addr)

// Driver structure
typedef struct _driver {
    qubit       *qregs;
    char        *cregs;
    unsigned    qtotal;
    unsigned    ctotal;
    unsigned    pointer;
} driver;

driver *new_driver(unsigned qtotal, unsigned ctotal);
bool free_driver(driver *d);
bool valid_qreg(driver *d, int addr);
bool valid_creg(driver *d, int addr);
qubit get_qubit(driver *d, int addr);
bool set_qubit(driver *d, int addr, qubit q);
unsigned get_bit(driver *d, int addr);
bool set_bit(driver *d, int addr, unsigned b);
bool apply_X(driver *d, int addr);
bool apply_Y(driver *d, int addr);
bool apply_Z(driver *d, int addr);
int apply_M(driver *d, int addr, bool echo);
bool process_operation(driver *d, nyanOperation c, int arguments[], bool echo);
int process_algorithm(driver *d, list *a, int arguments[], bool echo);

// Creates a new driver
driver *new_driver(unsigned qtotal, unsigned ctotal) {
    driver *d = malloc(sizeof(driver));
    d->qtotal = qtotal;
    d->ctotal = ctotal;
    d->qregs = malloc(sizeof(qubit) * d->qtotal);
    d->cregs = calloc(sizeof(char), d->ctotal + RESERVED_REGS);
    d->pointer = 0;
    int i;
    for(i = 0; i < d->qtotal; i++) d->qregs[i] = ZERO;
    srand(time(NULL));
    return d;
}

// Frees a new driver
bool free_driver(driver *d) {
    if(!d) return false;
    free(d->cregs);
    free(d->qregs);
    free(d);
    return true;
}

// Checks if a qubit register has been allocated
bool valid_qreg(driver *d, int addr) {
    return addr >= 0 && addr < d->qtotal && d->qregs != NULL;
}

// Checks if a bit register has been allocated
bool valid_creg(driver *d, int addr) {
    return addr >= 0 && addr < d->ctotal + RESERVED_REGS && d && d->cregs != NULL;
}

// Gets a qubit from driver
qubit get_qubit(driver *d, int addr) {
    if(!valid_qreg(d, addr)) {
        print_driver_error(DRIVER_ERR_GET_QUBIT_NON_EXISTENT);
        return (qubit){0, 0};
    }
    return d->qregs[addr];
}

// Sets a qubit in driver
bool set_qubit(driver *d, int addr, qubit q) {
    if(!valid_qreg(d, addr)) {
        print_driver_error(DRIVER_ERR_SET_QUBIT_NON_EXISTENT);
        return false;
    }
    d->qregs[addr] = q;
    return true;
}

// Gets a bit from driver
unsigned get_bit(driver *d, int addr) {
    if(addr >= REG(d, 1)) {
        print_driver_error(DRIVER_ERR_GET_FORBIDDEN_BIT);
        return false;
    }
    if(!valid_creg(d, addr)) {
        print_driver_error(DRIVER_ERR_GET_BIT_NON_EXISTENT);
        return 0;
    }
    return d->cregs[addr];
}

// Sets a qubit in driver
bool set_bit(driver *d, int addr, unsigned b) {
    if(addr >= REG(d, 1)) {
        print_driver_error(DRIVER_ERR_SET_FORBIDDEN_BIT);
        return false;
    }
    if(!valid_creg(d, addr)) {
        print_driver_error(DRIVER_ERR_SET_BIT_NON_EXISTENT);
        return false;
    }
    d->cregs[addr] = b;
    return true;
}

// Applies the Hadamard gate
bool apply_H(driver *d, int addr) {
    qubit q = get_qubit(d, addr);
    if(!valid_qubit(q)) return false;
    set_qubit(d, addr, hadamard(q));
    return true;
}

// Applies the Pauli X gate
bool apply_X(driver *d, int addr) {
    qubit q = get_qubit(d, addr);
    if(!valid_qubit(q)) return false;
    set_qubit(d, addr, pauli_x(q));
    return true;
}

// Applies the Pauli Y gate
bool apply_Y(driver *d, int addr) {
    qubit q = get_qubit(d, addr);
    if(!valid_qubit(q)) return false;
    set_qubit(d, addr, pauli_y(q));
    return true;
}

// Applies the Pauli Z gate
bool apply_Z(driver *d, int addr) {
    qubit q = get_qubit(d, addr);
    if(!valid_qubit(q)) return false;
    set_qubit(d, addr, pauli_z(q));
    return true;
}

// Applies the Measurement gate
int apply_M(driver *d, int addr, bool echo) {
    qubit q = get_qubit(d, addr);
    if(!valid_qubit(q)) return -1;
    if(echo)printf("[MEASUREMENT] Qubit %i: %s\n", addr, show_qubit(q));
    // Jumps one number in the stream to avoid repetition
    rand();
    int bit = abs(((double)rand() / RAND_MAX) > pow_complex(q.zero, 2).r);
    set_qubit(d, addr, bit ? ONE : ZERO);
    return bit;
}

// Process an operation
bool process_operation(driver *d, nyanOperation c, int arguments[], bool echo) {
    if(!d) return false;
    switch(c.code) {
        case OP_END:
            d->cregs[REG(d, 1)] = get_bit(d, c.specials[0]
                ? REG(d, c.values[0]) : c.values[0]);
            return false;
        case OP_MOV:            
            set_bit(d, c.specials[1]
                ? REG(d, c.values[1]) : c.values[1],
                get_bit(d, c.specials[0]
                    ? REG(d, c.values[0]) : c.values[0]));
            d->pointer++;
            return true;
        case OP_PUT:
            set_bit(d, c.specials[0]
                ? REG(d, c.values[0]) : c.values[0], c.specials[1]
                    ? arguments[c.values[1]] : c.values[1]);
            d->pointer++;
            return true;
        case OP_CMP:            
            set_bit(d, REG(d, 0), 
                get_bit(d, REG(d, 0)) > get_bit(d, c.specials[0]
                    ? REG(d, c.values[0]) : c.values[0]) ? 1
                : (get_bit(d, REG(d, 0)) == get_bit(d, c.specials[0]
                    ? REG(d, c.values[0]) : c.values[0]) ? 0
                : -1));            
            d->pointer++;
            return true;
        case OP_JE:
            if(get_bit(d, REG(d, 0)) == 0) d->pointer = c.values[0]; 
            else d->pointer++;
            return true;
        case OP_JNE:
            if(get_bit(d, REG(d, 0)) != 0) d->pointer = c.values[0]; 
            else d->pointer++;
            return true;
        case OP_JG:
            if(get_bit(d, REG(d, 0)) == 1) d->pointer = c.values[0]; 
            else d->pointer++;
            return true;
        case OP_JGE:
            if(get_bit(d, REG(d, 0)) == 0 || get_bit(d, 1) == 1) d->pointer = c.values[0]; 
            else d->pointer++;
            return true;
        case OP_JL:
            if(get_bit(d, REG(d, 0)) == -1) d->pointer = c.values[0]; 
            else d->pointer++;
            return true;
        case OP_JLE:
            if(get_bit(d, REG(d, 0)) == 0 || get_bit(d, REG(d, 0)) == -1) d->pointer = c.values[0]; 
            else d->pointer++;
            return true;
        case OP_ADD:
            set_bit(d, c.specials[0]
                ? REG(d, c.values[0]) : c.values[0], get_bit(d, c.specials[0]
                    ? REG(d, c.values[0]) : c.values[0]) + get_bit(d, c.specials[1]
                    ? REG(d, c.values[1]) : c.values[1]));
            d->pointer++;
            return true;
        case OP_SUB:
            set_bit(d, c.specials[0]
                ? REG(d, c.values[0]) : c.values[0], get_bit(d, c.specials[0]
                    ? REG(d, c.values[0]) : c.values[0]) - get_bit(d, c.specials[1]
                    ? REG(d, c.values[1]) : c.values[1]));
            d->pointer++;
            return true;
        case OP_MUL:
            set_bit(d, c.specials[0]
                ? REG(d, c.values[0]) : c.values[0], get_bit(d, c.specials[0]
                    ? REG(d, c.values[0]) : c.values[0]) * get_bit(d, c.specials[1]
                    ? REG(d, c.values[1]) : c.values[1]));
            d->pointer++;
            return true;
        case OP_DIV:
            set_bit(d, c.specials[0]
                ? REG(d, c.values[0]) : c.values[0], get_bit(d, c.specials[0]
                    ? REG(d, c.values[0]) : c.values[0]) / get_bit(d, c.specials[1]
                    ? REG(d, c.values[1]) : c.values[1]));
            d->pointer++;
            return true;
        case OP_M:
            set_bit(d, REG(d, 0), apply_M(d, c.values[0], echo));
            d->pointer++;
            return true;
        case OP_H:
            apply_H(d, c.values[0]);
            d->pointer++;
            return true;
        case OP_X:
            apply_X(d, c.values[0]);
            d->pointer++;
            return true;
        case OP_Y:
            apply_Y(d, c.values[0]);
            d->pointer++;
            return true;
        case OP_Z:
            apply_Z(d, c.values[0]);
            d->pointer++;
            return true;
    }
}

// Process an algorithm
int process_algorithm(driver *d, list *a, int arguments[], bool echo) {
    if(!d || !a) return -1;
    d->pointer = 0;
    int i = false;
    do {
        i = process_operation(d, get_val_from_list(a, d->pointer), arguments, echo);
    } while(i == true);
    if(echo) printf("Algorithm finished with return code %i.\n", d->cregs[REG(d, 1)]);
    return d->cregs[REG(d, 1)];
}

#endif