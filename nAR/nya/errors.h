#ifndef NYANCAT_NYA_ERRORS
#define NYANCAT_NYA_ERRORS

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#define MAX_ERROR_LENGTH    1<<8

char nyanDriverErrors[][MAX_ERROR_LENGTH] = {
    "Attempt to access the value of a non-allocated quantum register.",
    "Attempt to access the value of a non-allocated classical register.",
    "Attempt to access the value of a reserved classical register.",
    "Attempt to set the value of a non-allocated quantum register.",
    "Attempt to set the value of a non-allocated classical register.",
    "Attempt to set the value of a reserved classical register.",
    "Attempt to run instruction with invalid arguments."
};

typedef enum _nyanBuildError {
    BUILD_ERR_NO_ERRORS = -1,
    BUILD_ERR_GENERAL_NOT_NYA,
    BUILD_ERR_GENERAL_ILLEGAL_CHAR,
    BUILD_ERR_ARGUMENT_ILLEGAL,
    BUILD_ERR_ARGUMENT_UNKNOWN,
    BUILD_ERR_ARGUMENT_DUPLICATED,
    BUILD_ERR_ARGUMENT_NO_COMMA,
    BUILD_ERR_ARGUMENT_UNNECESSARY,
    BUILD_ERR_ARGUMENT_NO_ANGLE_BRACKET,
    BUILD_ERR_ARGUMENT_LOOSE_REFERENCE,
    BUILD_ERR_LABEL_ILLEGAL,
    BUILD_ERR_LABEL_UNKNOWN,
    BUILD_ERR_LABEL_DUPLICATED,
    BUILD_ERR_TASK_ILLEGAL,
    BUILD_ERR_TASK_UNKNOWN,
    BUILD_ERR_TASK_UNEXPECTED_PARAM,
    BUILD_ERR_TASK_MISSING_PARAMS,
    BUILD_ERR_TASK_EXCESSIVE_PARAMS,
    BUILD_ERR_REFERENCE_EMPTY,
    BUILD_ERR_REFERENCE_NO_SQUARE_BRACKET,
    BUILD_ERR_REFERENCE_NO_QUBIT,
    BUILD_ERR_REFERENCE_NO_VALUE,
    BUILD_ERR_REFERENCE_NO_ARGUMENT,
    BUILD_ERR_REFERENCE_NO_LABEL,
} nyanBuildError;

char nyanBuildErrors[][MAX_ERROR_LENGTH] = {
    "There isn't a .nya script named that way.",
    "There's a purposeless loose character on your code.",
    "It isn't allowed to declare arguments that way! (Maybe you're doing two stuff in one line?)",
    "It doesn't seem like there's a declared argument with that name.",
    "There's a cloned argument.",
    "Your arguments declaration is missing a comma!",
    "There's an unecessary character on your arguments declaration. Take it out!",
    "It seems that you forgot to \"close\" your arguments declaration. Put an angle bracket there!",
    "There's a loose argument on your code. Did you mean to call a task?",
    "It isn't allowed to declare labels that way! (Maybe you're doing two stuff on the same line?)",
    "It doesn't seem like there's a label with that name.",
    "There's a cloned label.",
    "It isn't allowed to call tasks that way! (Maybe you're doing two stuff on the same line?)",
    "It doesn't seem like there's a task with that name.",
    "The called task doesn't ask for that type of parameter.",
    "The task you've called asks for more parameters than you provided.",
    "Too much parameters for the task you've called.",
    "Your reference is empty. Did you forget to put a register in there?",
    "It seems that you forgot to \"close\" your reference. Put an angle bracket there!",
    "You cannot get the value of a qubit wihtout measuring it! Use the \"m\" task instead.",
    "You are trying to get a value from another value. Specify a classical register or remove the square brackets.",
    "Arguments are not allowed on references. You must specify a classical register.",
    "Labels are not allowed on references. I mean, what are you even trying to do?",
};

typedef enum _nyanDriverError {
    DRIVER_ERR_GET_QUBIT_NON_EXISTENT,
    DRIVER_ERR_GET_BIT_NON_EXISTENT,
    DRIVER_ERR_GET_FORBIDDEN_BIT,
    DRIVER_ERR_SET_QUBIT_NON_EXISTENT,
    DRIVER_ERR_SET_BIT_NON_EXISTENT,
    DRIVER_ERR_SET_FORBIDDEN_BIT,
    DRIVER_ERR_INVALID_ARGUMENT
} nyanDriverError;

void print_build_error(nyanBuildError err) { printf("[Build Error] %s\n", nyanBuildErrors[err]); }
void print_driver_error(nyanDriverError err) { printf("[Driver Error] %s\n", nyanDriverErrors[err]); }
#endif