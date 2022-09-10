# Titanite bytecode compiler

Following along the book [Crafting interpreters](https://craftinginterpreters.com/contents.html).

# Requirements

* [GCC](https://gcc.gnu.org/)

## Install gcc on ubuntu 20.04

```sh
sudo apt install build-essential
gcc --version # check if the installation succeeded
```

# Usage

Go to the [Autoconfig explanation](./AUTOCONFIG.md) for more info.

If you are not interested run the following:

```sh
sudo apt install autoconf
autoreconf -i
./configure
make
./titanite_compiler ./example.ti
```

# Done

1. Chunk array (implement dynamicly growing/shrinking array for our compiler)

## Contributing

Look at the list of TODOs below to find something you are interested in or just go through the code to find something.

## Guideline

### Specify why adding a dependency.

We want to pollute are files as little as possible. This means, if you add a header file for the first time in the c file from the standard c library, write down why you added it.

For example:

```c
#include <string.h> // for memcpy
```

# TODOs

There are a lot of TODO types. Look through them and search for those in the code, if you find a specific TODO type interesting.

* TODO(perf): search for this if you want to improve the performance of the language
* TODO(memory): search for this if you want to make the language consume less memory
* TODO(typing): search for this if you want to improve the typing/error messaging of the language
* TODO(needed): this indicates an important TODO, they are usually things that go wrong when you would write a lof of code (e.g. the stack size is currently limited and does not grow dynamically)
* TODO(tweak): Nice to have, but not very relevant
* TODO(error-handling): Improvements for better error messaging for the user. Somewhat important.
* TODO(threading): Improvements that provide better thread safety (or thread safety at all). Not really important until we get threads going, if we ever do.

# Important things that are missing

## Implement const keyword

Well this is very self-explanatory. you can reassign a different value to a variable which for declared with `let`. For example this is valid.

```rs
let a = 2;
a = 3;
```

But we also want a const keyword, which has to be initialized the moment it is declared. For example:

```js
const a; // this is illegal and should throw and error

const b = 3;
b = 4 // this is illegal and should throw and error

const c = 6; // we can only use this variable later
let d = c+3; // this is fine.
```

## Allow more that 256 variables at the same time.

Right now the number of local variable of the compiler cannot be more than 256, which is restricted by this variable in `common.h`.

```c
#define UINT8_COUNT (UINT8_MAX + 1)
```

and this typedefinition in `compiler.c` which uses the previous constant as the array length of the list where the locals get saved.

```c
typedef struct
{
    Local locals[UINT8_COUNT]; // this should start grow dynamically.
    int localCount;
    int scopeDepth;
} Compiler;
```

The restriction is enforced in the addLocal function. This could be a starting point to implement this feature.

```c
/**
 * This initializes the next available Local in the compiler’s array of variables.
 * It stores the variable’s name and the depth of the scope that owns the variable.
 */
static void addLocal(Token name)
{
    // If a 257th variable would be added right now, this if branch prevents it by throwing a compiler error.
    if (current->localCount == UINT8_COUNT)
    {
        error("Too many local variables in function.");
        return;
    }
    Local *local = &current->locals[current->localCount++];
    local->name = name;
    local->depth = current->scopeDepth;
}
```

## Line encoding

Our encoding of line information is hilariously wasteful of memory. Given that a series of instructions often correspond to the same source line, a natural solution is something akin to run-length encoding of the line numbers.

Devise an encoding that compresses the line information for a series of instructions on the same line. Change writeChunk() to write this compressed form, and implement a getLine() function that, given the index of an instruction, determines the line where the instruction occurs.

Hint: It’s not necessary for getLine() to be particularly efficient. Since it is called only when a runtime error occurs, it is well off the critical path where performance matters.

## Too little space for constants

Because OP_CONSTANT uses only a single byte for its operand, a chunk may only contain up to 256 different constants. That’s small enough that people writing real-world code will hit that limit. We could use two or more bytes to store the operand, but that makes every constant instruction take up more space. Most chunks won’t need that many unique constants, so that wastes space and sacrifices some locality in the common case to support the rare case.

To balance those two competing aims, many instruction sets feature multiple instructions that perform the same operation but with operands of different sizes. Leave our existing one-byte OP_CONSTANT instruction alone, and define a second OP_CONSTANT_LONG instruction. It stores the operand as a 24-bit number, which should be plenty.

Implement this function:

void writeConstant(Chunk* chunk, Value value, int line) {
  // Implement me...
}
It adds value to chunk’s constant array and then writes an appropriate instruction to load the constant. Also add support to the disassembler for OP_CONSTANT_LONG instructions.

Defining two instructions seems to be the best of both worlds. What sacrifices, if any, does it force on us?

## Own realloc function.

I've already implemented my own `malloc`, `realloc`, `calloc` and `free`. But mine are not thread safe. Altough they are very memory safe and they are 8-bit-aligned.