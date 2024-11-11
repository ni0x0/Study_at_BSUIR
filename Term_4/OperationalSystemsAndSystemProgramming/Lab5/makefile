#makefile
CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic -g -pthread 
INCLUDES = -I.
.PHONY : clean

all: oslab5
oslab5: oslab5.c optional.c optional.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) oslab5.c optional.c -o oslab5
clean:
	rm -f oslab5 
