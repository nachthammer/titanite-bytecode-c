#include <stdarg.h> // for va_list
#include <stdio.h>  // for printf
#include <string.h> // for memcpy

#include "common.h"
#include "debug.h"
#include "object.h"
#include "memory.h"
#include "vm.h"
#include "compiler.h"
#include "value.h"

VM vm;

static void resetStack()
{
    vm.stackTop = vm.stack;
}

static void runtimeError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}

void initVM()
{
    resetStack();
    vm.objects = NULL;
    initTable(&vm.globals);
    initTable(&vm.strings);
}

void freeVM()
{
    freeTable(&vm.strings);
    freeTable(&vm.globals);
    freeObjects();
}

void push(Value value)
{
    *(vm.stackTop) = value;
    vm.stackTop++;
}

Value pop()
{
    vm.stackTop--;
    return *vm.stackTop;
    // return *(--vm.stackTop); // is this equivalent
}

static Value peek(int distance)
{
    return vm.stackTop[-1 - distance];
}

static bool isFalsey(Value value)
{
    if (!IS_BOOL(value))
    {
        runtimeError("Operand must be a boolean.");
        return INTERPRET_RUNTIME_ERROR;
    }
    return !AS_BOOL(value);
}

static void concatenate()
{
    ObjString *b = AS_STRING(pop());
    ObjString *a = AS_STRING(pop());

    int length = a->length + b->length;
    char *chars = ALLOCATE(char, length + 1);
    memcpy(chars, a->chars, a->length);
    memcpy(chars + a->length, b->chars, b->length);
    chars[length] = '\0';

    ObjString *result = takeString(chars, length);
    push(OBJ_VAL(result));
}

// TODO(perf): make bytecode dispatch more efficient: direct threaded code; jump table; computed goto
static InterpretResult run()
{
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_STRING() AS_STRING(READ_CONSTANT())
#define BINARY_OP(valueType, op)                        \
    do                                                  \
    {                                                   \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) \
        {                                               \
            runtimeError("Operands must be numbers.");  \
            return INTERPRET_RUNTIME_ERROR;             \
        }                                               \
        double b = AS_NUMBER(pop());                    \
        double a = AS_NUMBER(pop());                    \
        push(valueType(a op b));                        \
    } while (false)

    for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(vm.chunk,
                               (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            push(constant);
            printValue(constant);
            printf("\n");
            break;
        }
        case OP_POP:
        {
            pop();
            break;
        }
        case OP_DEFINE_GLOBAL:
        {
            ObjString *name = READ_STRING();
            tableSet(&vm.globals, name, peek(0));
            pop();
            break;
        }
        case OP_GET_GLOBAL:
        {
            ObjString *name = READ_STRING();
            Value value;
            // if the variable does not exist.
            if (!tableGet(&vm.globals, name, &value))
            {
                runtimeError("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            push(value);
            break;
        }
        case OP_SET_GLOBAL:
        {
            ObjString *name = READ_STRING();
            // if the variable does not exist.
            if (tableSet(&vm.globals, name, peek(0)))
            {
                tableDelete(&vm.globals, name);
                runtimeError("Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_PRINT:
        {
            printValue(pop());
            printf("\n");
            break;
        }
        case OP_NIL:
            push(NIL_VAL);
            break;
        case OP_TRUE:
            push(BOOL_VAL(true));
            break;
        case OP_FALSE:
            push(BOOL_VAL(false));
            break;
        case OP_EQUAL:
        {
            Value b = pop();
            Value a = pop();
            int ERROR_FLAG = 0;
            bool value_equals = valuesEqual(a, b, &ERROR_FLAG);
            if (ERROR_FLAG == 1)
            {
                fprintf(stderr, "COMPARE_ERROR: Cannot compare value of different types.\n");
                return INTERPRET_RUNTIME_ERROR;
            }
            push(BOOL_VAL(value_equals));
            break;
        }
        case OP_GREATER:
            BINARY_OP(BOOL_VAL, >);
            break;
        case OP_LESS:
            BINARY_OP(BOOL_VAL, <);
            break;
        case OP_ADD:
        {
            if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1)))
            {
                double b = AS_NUMBER(pop());
                double a = AS_NUMBER(pop());
                push(NUMBER_VAL(a + b));
            }
            else
            {
                runtimeError(
                    "Can only add two numbers. If you want to concatenate string use '++' instead.");
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_CONCATENATE:
        {
            if (IS_STRING(peek(0)) && IS_STRING(peek(1)))
            {
                concatenate();
            }
            else
            {
                runtimeError(
                    "Can only concatenate two strings.");
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
        case OP_SUBTRACT:
            BINARY_OP(NUMBER_VAL, -);
            break;
        case OP_MULTIPLY:
            BINARY_OP(NUMBER_VAL, *);
            break;
        case OP_DIVIDE:
            BINARY_OP(NUMBER_VAL, /);
            break;
        case OP_NOT:
            push(BOOL_VAL(isFalsey(pop())));
            break;
        // TODO(perf): no need to push and pop the stack, because the stack size stays the same.
        case OP_NEGATE:
        {
            if (!IS_NUMBER(peek(0)))
            {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }
            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;
        }
        case OP_RETURN:
        {
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_STRING
#undef BINARY_OP
}

InterpretResult interpret(const char *source)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(source, &chunk))
    {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
}