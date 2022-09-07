all: clean main.o debug.o chunk.o memory.o
	gcc -o main main.o debug.o chunk.o memory.o -Wall -Wextra

main.o: main.c chunk.h common.h debug.h
	gcc -c main.c

debug.o: debug.c debug.h
	gcc -c debug.c

chunk.o: chunk.c chunk.h memory.h
	gcc -c chunk.c

memory.o: memory.c memory.h
	gcc -c memory.c


clean:
	rm -f main main.o debug.o chunk.o memory.o