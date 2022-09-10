# Titanite bytecode compiler

Following along the book [Crafting interpreters](https://craftinginterpreters.com/contents.html).

# Requirements

* [GCC](https://gcc.gnu.org/)

## Install gcc on ubuntu 20.04

```sh
sudo apt install build-essential
gcc --version
```

# Usage

Invoke the command in the Makefile. More information about Makefiles [here](https://www.gnu.org/software/make/manual/make.html)

We use the C17 standard.

```sh
make all
```

# Done

1. Chunk array (implement dynamicly growing/shrinking array for out compiler)

# TODOs

There are a lot of TODOs:

* TODO(perf): search for this if you want to improve the performance of the language
* TODO(memory): search for this if you want to make the language consume less memory
* TODO(typing): search for this if you want to improve the typing/error messaging of the language
* TODO(needed): this indicates an important TODO, they are usually things that go wrong when you would write a lof of code (e.g. the stack size is currently limited and does not grow dynamically)
* TODO(tweak): Nice to have, but not very relevant
* TODO(error-handling): Improvements for better error messaging for the user. Somewhat important.

# Important things that are missing

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