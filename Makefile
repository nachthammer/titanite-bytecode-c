# Compiler
CC = gcc

# Compiler flags
CFLAGS = -std=c17 -Wall -Wextra

all: clean main.o debug.o chunk.o memory.o value.o vm.o compiler.o scanner.o object.o table.o
	$(CC) -o main main.o debug.o chunk.o memory.o value.o vm.o compiler.o scanner.o object.o table.o $(CFLAGS)

main.o: main.c chunk.h common.h debug.h vm.h
	$(CC) -c main.c

debug.o: debug.c debug.h value.h
	$(CC) -c debug.c

chunk.o: chunk.c chunk.h memory.h
	$(CC) -c chunk.c

memory.o: memory.c memory.h
	$(CC) -c memory.c

value.o: value.c memory.h value.h
	$(CC) -c value.c

vm.o: vm.c vm.h common.h debug.h compiler.h
	$(CC) -c vm.c

compiler.o: compiler.c compiler.h common.h compiler.h scanner.h
	$(CC) -c compiler.c

scanner.o: scanner.c scanner.h common.h
	$(CC) -c scanner.c

object.o: object.c memory.h object.h value.h vm.h
	$(CC) -c object.c

table.o: table.c table.h memory.h object.h value.h
	$(CC) -c table.c


clean:
	rm -f main main.o debug.o chunk.o memory.o value.o