#makefile
CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic -g -pthread 
INCLUDES = -I.
.PHONY : clean

all: server client
server: server.c optionals.c optionals.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) server.c optionals.c -o server
client: client.c optionals.c optionals.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) client.c optionals.c -o client
clean:
	rm -f server client
