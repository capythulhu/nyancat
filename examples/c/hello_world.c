/*
    You can compile this algorithm by running `gcc examples/c/hello_world.c -lm`.
    This algorithm can print two different messages, depending on
    the result of a qubit's measurement.
*/

#define STDLIB_H
#include <stdlib.h>

#define STDIO_H
#include <stdio.h>

#include "../../nAR/nya/driver.h"

int main(void) {
    // Allocates driver
    driver *d = new_driver(1, 0);
    // Loads script and gets necessary count of registers and qubits
    qscript r = load_script("examples/nya/random_bit", false);
    // Runs loaded algorithm
    int bit = process_simple_algorithm(d, r.algorithm, false);
    // Prints message
    printf("%s world!", bit ? "Hello" : "Goodbye");
}