# Titanite bytecode compiler

Following along the book [Crafting interpreters].

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