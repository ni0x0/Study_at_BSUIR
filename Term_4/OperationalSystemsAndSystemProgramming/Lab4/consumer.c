#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>  
#include <string.h>
#include <fcntl.h>
#include <signal.h>

#include "msg_info.h"

/////////////////////////// FUNCTIONS  //////////////////////////////////////

void exit_sig_handler (int sign);

void process_msg();

////////////////////////////// VARIABLES ////////////////////////////// 
struct message consumed_msg; 

// Sinal Handling
struct sigaction exit_sigaction;

// FLAGS
volatile sig_atomic_t exit_fl = 0;

// Shared Memory
struct message_queue_info * sh_mem_ptr;

// Semaphores
sem_t *full_cond_sem;
sem_t *empty_cond_sem;
sem_t *mutex_sem;


//// // MAIN // //
int main (int argc, char *argv[])
{
    //printf("\nCCCCC{%s}{%s}CCCCCCC\n", argv[0], argv[1]);
    printf("\nConsumer _%s_ got sh mem id:%s and started to consume\n", argv[0], argv[1]);

    // Attach Memory
    sh_mem_ptr = (struct message_queue_info *)shmat(atoi(argv[1]), NULL, 0);
    if (sh_mem_ptr == (void*)-1) {
        perror("shmat");
        exit(1);
    }

    // Signal Handling Setting
    exit_sigaction.sa_handler = exit_sig_handler;
    sigaction(SIGUSR1, &exit_sigaction, NULL);


    // Semaphores
    full_cond_sem = sem_open(FULL_COND_SEM_N, O_CREAT, 0644, 1);
    empty_cond_sem = sem_open(EMPTY_COND_SEM_N, O_CREAT, 0644, MSGS_MAX_NUM - 1);
    mutex_sem = sem_open(MUTEX_SEM_NAME, O_CREAT, 0644, 1);

    if (full_cond_sem == SEM_FAILED || empty_cond_sem == SEM_FAILED || mutex_sem == SEM_FAILED) {
        fprintf(stderr, "Semaphores Open ERROR");
        exit(-1);
    }


    int num; 

    while (1) {

        if (exit_fl == 1) {
            break;
        }  
        //sem_getvalue(full_cond_sem, &num);
        //printf("\n%s waits %d\n", argv[0], num);
        sem_wait(full_cond_sem);
        if (exit_fl == 1) {
            break;
        }

        sem_wait(mutex_sem);
        process_msg();
        sem_post(mutex_sem);

        sem_post(empty_cond_sem);
        sem_wait(mutex_sem);
        printf("\n%s processed MSG:\n{%d}{%d}{%d}{ ", argv[0], consumed_msg.ttype, 
                   consumed_msg.hhash, consumed_msg.ssize);
            for (int j = 0; j < consumed_msg.ssize; j++) {
                printf(" %d ", consumed_msg.ddata[j]);
            }
            printf(" }\n");
            sem_post(mutex_sem);
        sleep(9);
        if (exit_fl == 1) {
            break;
        }  
    }



    /*
    sem_getvalue(empty_cond_sem, &num);
    printf("\n[%s]empty:%d", argv[0], num);
    sem_getvalue(full_cond_sem, &num);
    printf("\nfull:%d\n", num);
    */

    sem_close(full_cond_sem);
    sem_close(empty_cond_sem);
    sem_close(mutex_sem);

    int detach_stat = 1;
    detach_stat = shmdt(sh_mem_ptr);
    if(detach_stat == -1){
        fprintf(stderr, "SHM DETACH ERROR\n");
    }

    //printf("\nEXIT[%s]\n", argv[0]);


    exit(0);
}


void exit_sig_handler(int sign) {
    //printf("\nI, cons, got email to ex\n");
    exit_fl = 1;
}


void process_msg(){
    consumed_msg = sh_mem_ptr->msg_buff[sh_mem_ptr->head_ind];
    sh_mem_ptr->head_ind++;

    if (sh_mem_ptr->head_ind == MSGS_MAX_NUM) {
        sh_mem_ptr->head_ind = 0;
    }

    sh_mem_ptr->delled_msgs_counter++;
    sh_mem_ptr->msgs_curr_num--;
}









