#ifndef titanite_chunk_h
#define titanite_chunk_h

#include "common.h"
#include "value.h"

// TODO(perf): implement !=, <=, >= (OP_NOT_EQUAL, OP_LE, OP_GE)
typedef enum
{
    // control flow
    OP_JUMP_IF_FALSE,
    OP_JUMP,
    OP_LOOP,
    // other
    OP_CONSTANT,
    OP_POP,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_PRINT,
    OP_CONCATENATE,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_NEGATE,
    OP_RETURN,
} OpCode;

typedef struct
{
    int count;
    int capacity;
    uint8_t *code;
    int *lines;
    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line);
int addConstant(Chunk *chunk, Value value);
void freeChunk(Chunk *chunk);

#endif