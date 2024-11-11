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
#include <signal.h>
#include <fcntl.h>
#include <time.h>
//#include <pthread.h>

#include "msg_info.h"

/////////////////////////// FUNCTIONS  //////////////////////////////////////

void exit_sig_handler(int sign);

void generate_msg();
void add_msg_to_queue();


////////////////////////////// VARIABLES ///////////////////////////// 
 
struct message generated_msg;  
int p_num = 0;
 
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

//// // // MAIN // //
int main (int argc, char *argv[])
{
    printf("\nProducer _%s_ got sh mem id:%s and started to produce\n", argv[0], argv[1]);

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


    /*
    generate_msg();
    sh_mem_ptr->msg_buff[11] = generated_msg;
    generate_msg();  
    sh_mem_ptr->msg_buff[12] = generated_msg;
    */

    
    int num;
    srand(time(NULL));

    while (1) {

        generate_msg();

        if (exit_fl == 1) {
           break;
        }

        //sem_getvalue(empty_cond_sem, &num);
        //printf("\n%s waits %d\n", argv[0], num);
        sem_wait(empty_cond_sem);
        if (exit_fl == 1) {
            break;
        }

        sem_wait(mutex_sem);
        add_msg_to_queue();
        sem_post(mutex_sem);

        sem_post(full_cond_sem);

        sem_wait(mutex_sem);
        printf("\n%s added msg\n", argv[0]);
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


void exit_sig_handler(int sign){
    //printf("\nI, prod, got email to ex\n");
    exit_fl = 1;
}


void generate_msg() {

    int rand_num = 0;
    do {
        //srand(time(NULL));
        rand_num = rand() % MSIZE;
    } while (rand_num == 0 || rand_num == MSIZE);
    //printf("___%d___", rand_num);

    generated_msg.ttype = sh_mem_ptr->added_msgs_counter;
    generated_msg.ssize = (unsigned char)rand_num;
    generated_msg.hhash = 0; 
    for (int i = 0; i < rand_num; i++) {
        unsigned char r_n = (unsigned char)(rand() % MSIZE);
        generated_msg.ddata[i] = r_n;
        generated_msg.hhash += r_n;
    }

}


void add_msg_to_queue() {

    sh_mem_ptr->msg_buff[sh_mem_ptr->tail_ind] = generated_msg;
    sh_mem_ptr->tail_ind++;
    if (sh_mem_ptr->tail_ind == MSGS_MAX_NUM) {
        sh_mem_ptr->tail_ind = 0;
    }

    sh_mem_ptr->added_msgs_counter++;
    sh_mem_ptr->msgs_curr_num++;
}













    /*
    printf("\n---Messages Buff Stats:---\n"
           "Number of Messages in buffer:%d\n"
           "Number of Added messages:%d\nNumber of Processed Messages:%d\n",
           sh_mem_ptr->msgs_curr_num, sh_mem_ptr->added_msgs_counter, sh_mem_ptr->delled_msgs_counter);

        printf("\nHead&Tail nums:[%d][%d]\n", sh_mem_ptr->head_ind, sh_mem_ptr->tail_ind);

        for (int i = sh_mem_ptr->head_ind; i != sh_mem_ptr->tail_ind;) {
            printf("_msg[%d] - {%d}{%d}{%d}{ ", i, sh_mem_ptr->msg_buff[i].ttype, 
                   sh_mem_ptr->msg_buff[i].hhash, sh_mem_ptr->msg_buff[i].ssize);
            for (int j = 0; j < sh_mem_ptr->msg_buff[i].ssize; j++) {
                printf(" %d ", sh_mem_ptr->msg_buff[i].ddata[j]);
            }
            printf(" }\n");

            i++;
            if (i == MSGS_MAX_NUM) {
                i = 0;
            }
        }
        */
