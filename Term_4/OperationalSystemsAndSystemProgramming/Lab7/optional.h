#ifndef OPTIONALS
#define OPTIONALS

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>  
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <malloc.h>
#include <pthread.h>

#define MSIZE 180
#define DDATA_S ((MSIZE + 3)/4)*4
#define MSGS_STANDARD_NUM 13

#define PRODUCERS_MAX_NUM 13
#define CONSUMERS_MAX_NUM 13

/////////////////////////// STRUCTURES ///////////////////////// 
 
/// Message Struct 
struct Message{
    char ttype;
    int hhash;
    unsigned char ssize;    
    unsigned char ddata [DDATA_S];
};

/// Information about Queue of Messages 
struct Message_queue_info{

    //struct message msg_buff [MSGS_MAX_NUM];
    struct Message * msg_buff;

    //struct message *head_ptr;
    //struct message *tail_ptr;
    int head_ind;
    int tail_ind;

    int msgs_curr_num;
    int msgs_max_num;

    int added_msgs_counter;
    int delled_msgs_counter;
};

////////////////////////////// STRUCTURES ////////////////////////////// 

struct Producers_info {
    pthread_t pthreadd [PRODUCERS_MAX_NUM];
    int num;
    int prodi_is_working [PRODUCERS_MAX_NUM];
};

struct Consumers_info {
    pthread_t pthreadd [CONSUMERS_MAX_NUM];
    int num;
    int consi_is_working [CONSUMERS_MAX_NUM];
};


/////////////////////////// MSG_BUFFER ///////////////////////////////
 

///////////////////////// SEMAPHORE /////////////////////////////////// 

#define FULL_COND_SEM_N "/full_condit_sem"
#define EMPTY_COND_SEM_N "/empty_cond_sem"
#define MUTEX_SEM_NAME "/mutex_sem"


#endif
