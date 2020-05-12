#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#define ERR_GET_QUBIT_NON_EXISTENT  "Attempt to access the value of a non-allocated quantum register."
#define ERR_GET_BIT_NON_EXISTENT    "Attempt to access the value of a non-allocated classical register."
#define ERR_GET_FORBIDDEN_BIT       "Attempt to access the value of a reserved classical register."
#define ERR_SET_QUBIT_NON_EXISTENT  "Attempt to set the value of a non-allocated quantum register."
#define ERR_SET_BIT_NON_EXISTENT    "Attempt to set the value of a non-allocated classical register."
#define ERR_SET_FORBIDDEN_BIT       "Attempt to set the value of a reserved classical register."
#define ERR_INVALID_ARGUMENT        "Attempt to run instruction with invalid arguments."

void print_error(char *err) { printf("[ERROR] %s\n", err); }