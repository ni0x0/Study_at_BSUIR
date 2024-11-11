#makefile
CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic -g -pthread 
INCLUDES = -I.
.PHONY : clean

all: oslab71 oslab72
oslab71: oslab71.c optional.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) oslab71.c -o oslab71
oslab72: oslab72.c makefile
	$(CC) $(CFLAGS) $(INCLUDES) oslab72.c -o oslab72
clean:
	rm -f oslab71 oslab72
