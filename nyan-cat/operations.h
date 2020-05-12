#ifndef OPERATIONS_H
#define OPERATIONS_H

typedef struct _command {
    unsigned    op;
    unsigned    args[2];
} command;

enum operation {
    OP_END,
    OP_PUT,
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

// Ends the algorithm returning the value of the classical 
// register "addr"
#define END(addr)           ((command){OP_END,  {addr}})
// Move a value from the classical register "addr1" to "addr2"
#define MOV(addr1, addr2)   ((command){OP_MOV,  {addr1, addr2}})
// Writes a value to a classical register "addr"
#define PUT(addr, val)      ((command){OP_PUT,  {addr, val}})
// Compares the value of the classical register 1 with the
// classical register "addr"
#define CMP(addr)           ((command){OP_CMP,  {addr}})
// Jumps to a line "val" if the comparsion outputs that the 
// values are equal
#define JE(val)             ((command){OP_JE,   {val}})
// Jumps to a line "val" if the comparsion outputs that the 
// values are not equal
#define JNE(val)            ((command){OP_JNE,  {val}})
// Jumps to a line "val" if the comparsion outputs that the 
// first value is greater than the second
#define JG(val)             ((command){OP_JG,   {val}})
// Jumps to a line "val" if the comparsion outputs that the 
// first value is greater than or equal to the second
#define JGE(val)            ((command){OP_JGE,  {val}})
// Jumps to a line "val" if the comparsion outputs that the 
// first value is lower than the second
#define JL(val)             ((command){OP_JL,   {val}})
// Jumps to a line "val" if the comparsion outputs that the 
// first value is lower than or equal to the second
#define JLE(val)            ((command){OP_JLE,  {val}})
// Measures a quantum register and outputs the result
#define M(qaddr)            ((command){OP_M,    {qaddr}})
// Applies the hadamard gate to quantum register and outputs
// if the operation was successful
#define H(qaddr)            ((command){OP_H,    {qaddr}})
// Applies the X Pauli gate to quantum register and outputs
// if the operation was successful
#define X(qaddr)            ((command){OP_X,    {qaddr}})
// Applies the Y Pauli gate to quantum register and outputs
// if the operation was successful
#define Y(qaddr)            ((command){OP_Y,    {qaddr}})
// Applies the Z Pauli gate to quantum register and outputs
// if the operation was successful
#define Z(qaddr)            ((command){OP_Z,    {qaddr}})

#endif