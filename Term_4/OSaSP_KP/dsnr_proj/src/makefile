#makefile
CC = gcc
CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic -g 
INCLUDES = -I.

DSNR_CONTROLLER = ../build/dsnr_controller

DATABASE_FLAGS = -ldb
DUPSDETECTOR_FLAGS = -lcrypto

.PHONY : clean

all: $(DSNR_CONTROLLER)
$(DSNR_CONTROLLER): kursw.c daemon_opts.c daemon_opts.h database_opts.c database_opts.h hash_opts.c hash_opts.h makefile
	$(CC) $(CFLAGS) $(INCLUDES) kursw.c daemon_opts.c database_opts.c hash_opts.c -o $(DSNR_CONTROLLER) $(DATABASE_FLAGS) $(DUPSDETECTOR_FLAGS)
clean:
	rm -f $(DSNR_CONTROLLER)
	
	

