all: build

build:
	gcc -g -Wall -o simulator commands.c PCB.c list.c

run: build
	./simulator

valgrind: build
	valgrind --leak-check=full ./simulator

clean:
	rm -f simulator