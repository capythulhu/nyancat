#define STDLIB_H
#include <stdlib.h>

#define STDIO_H
#include <stdio.h>

#include "nyan-cat/driver.h"

int main(void) {
    // New driver with 1 quantum register and 2 classical
    // registers
    driver *d = new_driver(1, 2);

    // Target value
    unsigned value = 1;
    command alg_setQubit[] = 
    {
        // Puts the target value in the classical register "0"
        PUT(0, value),
        // Inverts the qubit "0"
        X(0),
        // Measures the qubit "0"
        M(0),
        // Moves the result to the classical register "1"
        MOV(REG(d, 0), 1),
        // Compares the classical register "0" with the output
        // of the measurement
        CMP(0),
        // Jumps back to line 1 (X(0)), if the values are not
        // equal
        JNE(1),
        // Ends the algorithm, outputing the measurement
        // result
        END(1)
    };

    process_algorithm(d, alg_setQubit, true);
    free_driver(d);
}