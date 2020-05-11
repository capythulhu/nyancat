#define STDLIB_H
#include <stdlib.h>

#define STDIO_H
#include <stdio.h>

#include "catbox/driver.h"

int main(void) {
    // New driver
    driver *d = new_driver(1);

    // Simple algorithm to set a qubit to the int 'q'
    int q = 0;
    command alg_setQubit[] = 
    { 
        X(0),
        CMP(0, q),
        JNE(0),
        M(0),
        MOV(0, 0),
        END(0)
    };

    process_algorithm(d, alg_setQubit, false);

    printf("Result: %i", d->cache[0]);
}