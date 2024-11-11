#makefile
CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic
.PHONY : clean

all: parent child
parent: Parent.c makefile
	$(CC) $(CFLAGS) Parent.c -o parent
child: Child.c makefile
	$(CC) $(CFLAGS) Child.c -o child
clean:
	rm -f parent child
