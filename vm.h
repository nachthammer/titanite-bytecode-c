#ifndef titanite_vm_h
#define titanite_vm_h

#include "chunk.h"
#include "table.h"
#include "value.h"

// TODO(needed): make stack grow dynamically
#define STACK_MAX 256 // more means, "stack overflow"

typedef struct
{
    Chunk *chunk;
    uint8_t *ip; // intruction pointer (points to the intruction that is about to be executed)
    Value stack[STACK_MAX];
    Value *stackTop;
    Table strings;
    Table globals;
    Object *objects;
} VM;

typedef enum
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM();
void freeVM();
InterpretResult interpret(const char *source);
// stack operations
void push(Value value);
Value pop();

#endif