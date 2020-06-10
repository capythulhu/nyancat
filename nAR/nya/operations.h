#ifndef NYANCAT_NYA_OPERATIONS
#define NYANCAT_NYA_OPERATIONS
#define MAX_NUM_LENGTH 1<<4
#define MAX_TASK_LENGTH 1<<2
#define MAX_PARAMS_COUNT 1<<2

typedef enum _nyanParam {
    TYPE_VOID,
    TYPE_VAL,
    TYPE_LABEL,
    TYPE_BIT,
    TYPE_QUBIT
} nyanParam;

typedef struct _nyanTask {
    char        name[MAX_TASK_LENGTH];
    nyanParam   parameters[MAX_PARAMS_COUNT];
} nyanTask;

typedef struct _nyanOperation {
    int     code;
    int     values[MAX_PARAMS_COUNT];
    bool    specials[MAX_PARAMS_COUNT];
} nyanOperation;

static const nyanTask nyanTasks[] = {
    { "end",    { TYPE_BIT } },
    { "mov",    { TYPE_BIT, TYPE_BIT } },
    { "put",    { TYPE_BIT, TYPE_VAL } },
    { "cmp",    { TYPE_BIT } },
    { "je",     { TYPE_LABEL } },
    { "jne",    { TYPE_LABEL } },
    { "jg",     { TYPE_LABEL } },
    { "jge",    { TYPE_LABEL } },
    { "jl",     { TYPE_LABEL } },
    { "jle",    { TYPE_LABEL } },
    { "add",    { TYPE_BIT, TYPE_BIT } },
    { "sub",    { TYPE_BIT, TYPE_BIT } },
    { "mul",    { TYPE_BIT, TYPE_BIT } },
    { "div",    { TYPE_BIT, TYPE_BIT } },
    { "m",      { TYPE_QUBIT } },
    { "h",      { TYPE_QUBIT } },
    { "x",      { TYPE_QUBIT } },
    { "y",      { TYPE_QUBIT } },
    { "z",      { TYPE_QUBIT } },
};

static const enum _nyanOperations {
    OP_END,
    OP_MOV,
    OP_PUT,
    OP_CMP,
    OP_JE,
    OP_JNE,
    OP_JG,
    OP_JGE,
    OP_JL,
    OP_JLE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_M,
    OP_H,
    OP_X,
    OP_Y,
    OP_Z
} nyanOperations;
#endif