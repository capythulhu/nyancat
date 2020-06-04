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

    algorithm a = load_script(argv[1]);
    free_driver(d);
}