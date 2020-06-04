#define STDLIB_H
#include <stdlib.h>

#define STDIO_H
#include <stdio.h>

#include "nyancat/nya/driver.h"
#include "nyancat/nya/lexer.h"

int main(int argc, char **argv) {
    // New driver with 1 quantum register and 2 classical
    // registers
    driver *d = new_driver(1, 2);

    // Target value
    unsigned value = atoi(argv[1]) & 1;

    algorithm a = load_script("examples/set_qubit");
    free_driver(d);
}