#ifndef OPERATIONS_H
#define OPERATIONS_H

typedef struct _command {
    unsigned    op;
    unsigned    args[4];
} command;

enum operation {
    OP_END,
    OP_MOV,
    OP_CMP,
    OP_JE,
    OP_JNE,
    OP_JG,
    OP_JGE,
    OP_JL,
    OP_JLE,
    OP_JMP,
    OP_M,
    OP_H,
    OP_X,
    OP_Y,
    OP_Z
};

#define END(val)            ((command){OP_END,  {val}})
#define MOV(caddr, addr)    ((command){OP_MOV,  {caddr, addr}})
#define CMP(qaddr, val)     ((command){OP_CMP,  {qaddr, val}})
#define JE(val)             ((command){OP_JE,   {val}})
#define JNE(val)            ((command){OP_JNE,  {val}})
#define JG(val)             ((command){OP_JG,   {val}})
#define JGE(val)            ((command){OP_JGE,  {val}})
#define JL(val)             ((command){OP_JL,   {val}})
#define JLE(val)            ((command){OP_JLE,  {val}})
#define M(qaddr)            ((command){OP_M,    {qaddr}})
#define H(qaddr)            ((command){OP_H,    {qaddr}})
#define X(qaddr)            ((command){OP_X,    {qaddr}})
#define Y(qaddr)            ((command){OP_Y,    {qaddr}})
#define Z(qaddr)            ((command){OP_Z,    {qaddr}})

#endif