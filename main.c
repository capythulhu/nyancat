#define STDLIB_H
#include <stdlib.h>

#define STDIO_H
#include <stdio.h>

#include "catbox/driver.h"

int main(void) {
    // New driver
    driver *d = new_driver(1, 1);

    command alg_setQubit[] = 
    {
        END(0)
    };

    process_algorithm(d, alg_setQubit, true);
    free_driver(d);
}