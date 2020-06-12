#define STDLIB_H
#include <stdlib.h>

/*
    You can compile this algorithm by running `gcc examples/c/bells_test.c -lm`.
    This algorithm will do N qubits measurements and output how many of them
    were measured "1".
*/

#define STDIO_H
#include <stdio.h>

#include "../../nAR/nya/driver.h"

int main(int argc, char **argv) {
    // Gets arguments from command line
    int i[MAX_PARAMS_COUNT] = {0};
    i[0] = atoi(argv[1]);
    // Loads script and gets necessary count of registers and qubits
    qscript r = load_script("examples/nya/bells_test", false);
    // Allocates driver
    driver *d = new_driver(1, 2);
    // Runs loaded algorithm
    int ones = process_algorithm(d, r.algorithm, i, false);
    // Calculates percentage
    float percent = (ones * 100.0f / i[0]);
    // Prints result and percentage
    printf("|0> = %i (%.2f%%)\n|1> = %i (%.2f%%)", i[0] - ones, 100.0f - percent, ones, percent);
}