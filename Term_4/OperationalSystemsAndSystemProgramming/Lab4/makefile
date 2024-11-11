#makefile
CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic -g -pthread
INCLUDES = -I.
.PHONY : clean

all: oslab4 producer consumer
oslab4: oslab4.c msg_info.c msg_info.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) oslab4.c msg_info.c -o oslab4
producer: producer.c msg_info.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) producer.c -o producer
consumer: consumer.c msg_info.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) consumer.c -o consumer
clean:
	rm -f oslab4 producer consumer
