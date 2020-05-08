#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#include "driver.h"

int main(void) {
    qubit *a = new_qubit(0, 1);
    qubit *b = new_qubit(1, 0);
    qubit *c = new_qubit(1, 1);
    printf("Is qubit A valid? %s\n", a ? "true" : "false");
    printf("Is qubit B valid? %s\n", b ? "true" : "false");
    printf("Is qubit C valid? %s\n", c ? "true" : "false");
}