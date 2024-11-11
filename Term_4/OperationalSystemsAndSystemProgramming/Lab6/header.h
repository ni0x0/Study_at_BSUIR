#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <string.h>


//#define PAGESIZE 4096
#define INDEX_DEFAULT_SIZE 1024   //2048 //4096  //64000  //128000      //
#define MEMSIZE_DEFAULT_NUM 4096
#define GRANULS_DEFAULT_NUM 8
#define THREADS_DEFAULT_NUM 5

#define INDEX_DEFAULT_MIN_SIZE 256
#define DEFAULT_FILE_NAME "unsorted_file"

#define START_TIME 15020
#define CURR_MODIF_JD 60082
#define ONE_DAY_SECONDS 86400
#define FRACT_MARK 0.5

struct index_s {
    double time_mark; // временная метка (модифицированная юлианская дата)
    uint64_t recno;   // первичный индекс в таблице БД
};

struct index_hdr_s {
    uint64_t recsords;      // количество записей
    struct index_s idx [];    // массив записей в количестве records
};


#define NOFNIN_ROW 5
#define NOFNIN_COLUMN 21
