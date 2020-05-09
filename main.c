#define STDLIB_H
#include <stdlib.h>

#define STDIO_H
#include <stdio.h>

#include "driver.h"

int main(void) {
    // New driver
    driver *d = new_driver(2);

    // Allocates qubits
    int q1 = allocate_qubit(d);
    int q2 = allocate_qubit(d);

    // Initializes qubits
    set_qubit(d, q1, ZERO);
    set_qubit(d, q2, ONE);

    // Puts qubits in superposition
    H(d, q1);
    H(d, q2);

    // Prints qubits
    printf("q1: %s\n%s\n", show_qubit(d, q1), show_percentages(d, q1));
    printf("q2: %s\n%s\n", show_qubit(d, q2), show_percentages(d, q2));

    printf("\n");

    // Measures qubits
    int b1 = M(d, q1);
    int b2 = M(d, q2);

    // Prints bits
    printf("b1: %i\n", b1);
    printf("b2: %i\n", b2);
}