#makefile
CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic -g -pthread
INCLUDES = -I.
.PHONY : clean

all: generator viewer sorter
generator: generator.c header.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) generator.c header.h -o generator
viewer: viewer.c header.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) viewer.c header.h -o viewer
sorter: sorter.c header.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) sorter.c header.h -o sorter
	
clean:
	rm -f generator
