#define STDLIB_H
#include <stdlib.h>

#define STDIO_H
#include <stdio.h>

#include "nyan-cat/driver.h"

int main(void) {
    // New driver
    driver *d = new_driver(1, 1);

    command alg_setQubit[] = 
    {
        X(2),
        PUT(REG(2), 5),
        END(REG(2))
    };

    process_algorithm(d, alg_setQubit, true);
    free_driver(d);
}