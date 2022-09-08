# Documentation

This talks about what every file does.

## Chunk

1. OpCode enums (OP_NOT, OP_ADD, ...)
2. struct Chunk + Chunk related utility functions (initChunk, freeChunk, ...)

A chunk has:

- a current `int count` (size),
- a current `int capacity`, if something is added and the current count would exceed the capacity the capacity has to be increased.
- a pointer to the `uint8_t* code`
- an integer array `int* lines`
- an array of constants which belong to this chunk `ValueArray constants`
