#define STDLIB_H
#include <stdlib.h>

#define STDIO_H
#include <stdio.h>

#include "nyancat/nya/lexer.h"

int main(int argc, char **argv) {
    algorithm a = load_script(argv[1]);
}