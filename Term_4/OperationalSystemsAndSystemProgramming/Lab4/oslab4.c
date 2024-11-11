#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>  
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include "msg_info.h"

#define SHM_STRKEY "shared_memory_key"


/////////////////////////// FUNCTIONS  //////////////////////////////////////
void datas_init();
void show_menu();

void add_producer();
void add_consumer();

void get_statistics();
void show_queue_stats();
void show_all_massages(); 
void show_all_producers();
void show_all_consumers();

void dell_all_producers();
void dell_all_consumers();

void set_some_msgs();

////////////////////////////// STRUCTURES ////////////////////////////// 

struct producers_info {
    pid_t producers_pids [PRODUCERS_MAX_NUM];
    int producers_num;
};

struct consumers_info {
    pid_t consumers_pids [CONSUMERS_MAX_NUM];
    int consumers_num;
};

////////////////////////////// VARIABLES ////////////////////////////// 

struct producers_info producers;
struct consumers_info consumers;

// Semaphores
sem_t *full_cond_sem;
sem_t *empty_cond_sem;
sem_t *mutex_sem;

// Shared memory variables
key_t sh_key;
int shm_id;

//void* shm_ptr;
struct message_queue_info *shm_ptr;


/// // MAIN // //
int main (int argc, char *argv[])
{
    datas_init();

// - CHOICE PART - 
    char choice;
    show_menu();
    while (1) {
        //int child_status;
        //wait(&child_status);
        printf("\nYou Can Enter Your Choice:\n");
        rewind(stdin);
        choice = getchar(); getchar();
 
        //printf("it is {%c}\n", choice);
        switch (choice) {
        case 'm' :
            show_menu();
            break; 
             
        // Add Producer
        case 'p' : 
            if (producers.producers_num == PRODUCERS_MAX_NUM) {
                fprintf(stderr, "\nMax number of Producers is Achieved\n");
            }
            else {
                add_producer();
            }
            break;

        // Dellete Producer
        case 'd' : 
            if (producers.producers_num > 0) {
                kill(producers.producers_pids[producers.producers_num - 1], SIGUSR1);
                producers.producers_num--;
                printf("\nLast created Producer is successfully deleted. %d Producers left\n", producers.producers_num);
            } else {
                fprintf(stderr, "\nNo Producers to delete\n");
            }
            break;

        // Add Consumer
        case 'c' : 
            if (consumers.consumers_num == CONSUMERS_MAX_NUM) {
                fprintf(stderr, "\nMax number of Consumers is Achieved\n");
            }
            else {
                add_consumer();
            }
            break;

        // Dellete Consumer
        case 'e' : 
            if (consumers.consumers_num > 0) {
                kill(consumers.consumers_pids[consumers.consumers_num - 1], SIGUSR1);
                consumers.consumers_num--;
                printf("\nLast created Consumer is successfully deleted. %d Consumers left\n", consumers.consumers_num);
            } else {
                fprintf(stderr, "\nNo Consumers to delete\n");
            }
            break;

        // Get Statistic
        case 's' : 
            get_statistics();
            break;
        case 'l' : 
            show_all_massages();
            break;

        case '\n' : 
            getchar();
            break;

        default:
            break;
        }


        if (choice == 'q') {
            break;
        }  
    } // end of main while

// Delete Child Proccesses
    dell_all_producers();
    dell_all_consumers();

// Detaching shared memory
    int detach_stat = 1;
    detach_stat = shmdt(shm_ptr);
    if(detach_stat == -1){
        fprintf(stderr, "SHM DETACH ERROR\n");
    }
    detach_stat = shmctl(shm_id, IPC_RMID, NULL);
    if(detach_stat == -1){
        fprintf(stderr, "SHM CTL ERROR\n");
    }

    sem_close(full_cond_sem);
    sem_close(empty_cond_sem);
    sem_close(mutex_sem);
    sem_unlink(FULL_COND_SEM_N);
    sem_unlink(EMPTY_COND_SEM_N);
    sem_unlink(MUTEX_SEM_NAME);

    exit(0);
}



/// ----------- Data initialization ----------------------------------------------------------------------------------------
void datas_init() {
    producers.producers_num = 0;
    consumers.consumers_num = 0;

// Shared Mem Actions
    // Generate key for shared memory
    sh_key = ftok(SHM_STRKEY, 'R');

    // Create shared memory
    shm_id = shmget(sh_key, sizeof(struct message_queue_info), IPC_CREAT | 0666); 
    if (shm_id == -1) {
        perror("SH MEM GET ERROR");
        exit(1);
    }
    printf("\nShared Memory successfully created. It's ID:%d\n", shm_id);

    // Attach shared memory 
    shm_ptr = (struct message_queue_info *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("SH MEM MAT ERROR");
        exit(1);
    }

// Shared Memory Datas Init
    shm_ptr->added_msgs_counter = 1;
    shm_ptr->delled_msgs_counter = 0;

    // {0123......} {...0123...}{......0123}{23......01}
    shm_ptr->head_ind = 0;
    shm_ptr->tail_ind = 1;

    struct message ch_msg = { 'D', 346, 5, "!MMa!"};
    shm_ptr->msg_buff[0] = ch_msg;
    shm_ptr->msgs_curr_num = 1;
    //set_some_msgs();


    // Semaphores
    sem_unlink(FULL_COND_SEM_N);
    sem_unlink(EMPTY_COND_SEM_N);
    sem_unlink(MUTEX_SEM_NAME);
    full_cond_sem = sem_open(FULL_COND_SEM_N, O_CREAT, 0644, 1);
    empty_cond_sem = sem_open(EMPTY_COND_SEM_N, O_CREAT, 0644, MSGS_MAX_NUM - 1);
    mutex_sem = sem_open(MUTEX_SEM_NAME, O_CREAT, 0644, 1);

    if (full_cond_sem == SEM_FAILED || empty_cond_sem == SEM_FAILED || mutex_sem == SEM_FAILED) {
        fprintf(stderr, "Semaphores Open ERROR");
        exit(-1);
    }

}


/// ----------- Show menu ----------------------------------------------------------------------------------------
void show_menu() {
    printf ("\nm - Show menu\n"
            "p - Create  producer \n"
            "c - Create consumer \n"
            "d - Delete producer \n"
            "e - Delete consumer \n"
            "s - Get Producers and Consumers statistic\n"
            "l - See Messages in buffer\n"
            "q - Quit \n");
}


/// ----------- Add Producer ---------------------------------------------------------------------------------------- 
void add_producer() {

        pid_t pid = fork();
        producers.producers_pids[producers.producers_num] = pid;

        if (pid == 0) {
            // Child proc code:
            char *child_argv[] = {"Prod_Name", "ShMemID",  NULL}; 
            char child_argv_name[10] = {"Prod_"};
            char child_argv_shmid[10] = {"id"};
                sprintf(child_argv_name + 5, "%d", producers.producers_num);
            child_argv[0] = child_argv_name;
                sprintf(child_argv_shmid, "%d", shm_id);
            child_argv[1] = child_argv_shmid;

            execve("./producer", child_argv, NULL);
        } else if (pid > 0) {
            // Parent proc code:
            printf("\nParent created Producer_%d_\n", pid);

            producers.producers_num++;

        } else {
            fprintf(stderr, "Producer Creation ERROR\n");
            exit(-1);
        }
}

/// ----------- Add Consumer ---------------------------------------------------------------------------------------- 
void add_consumer() {

        pid_t pid = fork();
        consumers.consumers_pids[consumers.consumers_num] = pid;

        if (pid == 0) {
            // Child proc code:
            char *child_argv[] = {"Cons_Name", "ShMemID",  NULL}; 
            char child_argv_name[10] = {"Cons_"};
            char child_argv_shmid[10] = {"id"};
                sprintf(child_argv_name + 5, "%d", consumers.consumers_num);
            child_argv[0] = child_argv_name;
                sprintf(child_argv_shmid, "%d", shm_id);
            child_argv[1] = child_argv_shmid;

            execve("./consumer", child_argv, NULL);
        } else if (pid > 0) {
            // Parent proc code:
            printf("\nParent created Consumer_%d_\n", pid);

            consumers.consumers_num++;

        } else {
            fprintf(stderr, "Consumer Creation ERROR\n");
            exit(-1);
        }
}

/// ----------- Get Statistic ----------------------------------------------------------------------------------------
void get_statistics() {

    // Show queue stats
    show_queue_stats();
    // Show all messages
    //show_all_massages();

    // Show producers
    show_all_producers();
    // Show consumers
    show_all_consumers();

}

/// ----------- Show Queue stats -------------------------------------------------------------------------------------------------------
void show_queue_stats() {
    sem_wait(mutex_sem);

    printf("\n---Messages Buff Stats:---\n"
           "Number of Messages in buffer:%d\n"
           "Number of Added messages:%d\nNumber of Processed Messages:%d\n",
           shm_ptr->msgs_curr_num, shm_ptr->added_msgs_counter, shm_ptr->delled_msgs_counter);

    sem_post(mutex_sem);
}


/// ----------- Show Messages ----------------------------------------------------------------------------------------
void show_all_massages() {
    sem_wait(mutex_sem);
    // (*head == *tail)
    if (((struct message_queue_info *)shm_ptr)->msgs_curr_num == 0) {
        printf("\nNo Messages in Buffer\n");
    }

    else {

        printf("\nHead&Tail inds:[%d][%d]\n", shm_ptr->head_ind, shm_ptr->tail_ind);


        for (int i = shm_ptr->head_ind, k = 0; k < shm_ptr->msgs_curr_num; k++) {
            printf("_MSG[%d]: {%d}{%d}{%d}{ ", i, shm_ptr->msg_buff[i].ttype, 
                   shm_ptr->msg_buff[i].hhash, shm_ptr->msg_buff[i].ssize);
            for (int j = 0; j < shm_ptr->msg_buff[i].ssize; j++) {
                printf(" %d ", shm_ptr->msg_buff[i].ddata[j]);
            }
            printf(" }\n");

            i++;
            if (i == MSGS_MAX_NUM) {
                i = 0;
            }
        } 
        

    }
    sem_post(mutex_sem);
}

/// ----------- Show producers and consumers ----------------------------------------------------------------------------------------
void show_all_producers() {
    if (producers.producers_num == 0) {
        printf("\n---No Working Producers---\n");
    }
    else {
        printf("\n---Working Producers:\n");
        for (int i = 0; i < producers.producers_num; i++) {
            printf("Producer_%d Pid: %d;\n", i, producers.producers_pids[i]);
        }

    }
}

void show_all_consumers() {
    if (consumers.consumers_num == 0) {
        printf("\n---No Working Consumers---\n");
    }
    else {
        printf("\n---Working Consumers:\n");
        for (int i = 0; i < consumers.consumers_num; i++) {
            printf("Consumer_%d Pid: %d;\n", i, consumers.consumers_pids[i]);
        }

    }
}

/// ----------- Dell producers and consumers----------------------------------------------------------------------------------------
void dell_all_producers() {
    for (int i = 0; i < producers.producers_num; i++) {
        kill(producers.producers_pids[i], SIGUSR1);
    }
}

void dell_all_consumers() {
    for (int i = 0; i < consumers.consumers_num; i++) {
        kill(consumers.consumers_pids[i], SIGUSR1);
    }
}








void set_some_msgs() {
    // {0123......} {...0123...}{......0123}{23......01}
    struct message ch_msg = { 'D', 346, 5, "!MMa!"};
    struct message ch_msg2 = { 'A', 3346, 4, "!jh!"};
    struct message ch_msg3 = { 'q', 306, 7, "CCHh!!c"};
    struct message ch_msg4 = { 'f', 32, 2, "Rd"};
    struct message ch_msg5 = { 'u', 45, 4, "!!MM"};

    // {0123......} 
     /*
    //shm_ptr->msg_buff[0] = ch_msg;
    shm_ptr->msg_buff[1] = ch_msg2;
    shm_ptr->msg_buff[2] = ch_msg3;
    shm_ptr->msg_buff[3] = ch_msg4;
    shm_ptr->msg_buff[4] = ch_msg5;
    shm_ptr->msgs_curr_num = 5;
    shm_ptr->head_ind = 0;
    shm_ptr->tail_ind = 5;  
     */ 

    // {...0123...}
    /*
    shm_ptr->msg_buff[3] = ch_msg;
    shm_ptr->msg_buff[4] = ch_msg2;
    shm_ptr->msg_buff[5] = ch_msg3;
    shm_ptr->msg_buff[6] = ch_msg4;
    shm_ptr->msg_buff[7] = ch_msg5;
    shm_ptr->msgs_curr_num = 5;
    shm_ptr->head_ind = 3;
    shm_ptr->tail_ind = 8; 
    */ 

    // {......0123}
    
    shm_ptr->msg_buff[8] = ch_msg;
    shm_ptr->msg_buff[9] = ch_msg2;
    shm_ptr->msg_buff[10] = ch_msg3;
    shm_ptr->msg_buff[11] = ch_msg4;
    shm_ptr->msg_buff[12] = ch_msg5;
    shm_ptr->msgs_curr_num = 5;
    shm_ptr->head_ind = 8;
    shm_ptr->tail_ind = 0; 
    
     
    // {23......01}
    /*
    shm_ptr->msg_buff[11] = ch_msg;
    shm_ptr->msg_buff[12] = ch_msg2;
    shm_ptr->msg_buff[0] = ch_msg3;
    shm_ptr->msg_buff[1] = ch_msg4;
    shm_ptr->msg_buff[2] = ch_msg5;
    shm_ptr->msgs_curr_num = 5;
    shm_ptr->head_ind = 11;
    shm_ptr->tail_ind = 3; 
    */

}










