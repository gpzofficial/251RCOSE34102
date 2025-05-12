CC = gcc
OBJS = main.o ossim.o

main: $(OBJS)
	gcc $^ -o main

test: _test.c
	gcc _test.c -o test

clean:
	rm -rf *.o
	rm -rf main
