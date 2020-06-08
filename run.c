#define STDLIB_H
#include <stdlib.h>

#define STDIO_H
#include <stdio.h>

#include "nyancat/nya/driver.h"

int main(int argc, char **argv) {
    // Gets arguments from command line
    int i, j[MAX_PARAMS_COUNT];
    for(i = 0; i < MAX_PARAMS_COUNT; i++) {
        j[i] = i < argc - 2 ? atoi(argv[i + 2]) : 0;
    }
    // Loads script and gets necessary count of registers and qubits
    nyanResult r = load_script(argv[1], false);
    // Allocates driver
    driver *d = new_driver(r.qtotal, r.ctotal);
    // Runs loaded algorithm
    printf("-> %i <-", process_algorithm(d, r.algorithm, j, false));
}