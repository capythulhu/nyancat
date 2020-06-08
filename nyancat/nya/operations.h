#ifndef NYANCAT_NYA_OPERATIONS
#define NYANCAT_NYA_OPERATIONS
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
    { "m",      { TYPE_QUBIT } },
    { "h",      { TYPE_QUBIT } },
    { "x",      { TYPE_QUBIT } },
    { "y",      { TYPE_QUBIT } },
    { "z",      { TYPE_QUBIT } },
};
#endif