
#include "optional.h"


/////////////////////////// FUNCTIONS  /////////////////////////////////////
void datas_init();
void show_menu();

void add_producer();
void * produce_proc(void * arg);

void add_consumer();
void * consume_proc(void * arg);

void get_statistics();
void show_queue_stats();

void show_all_messages(); 
void show_all_producers();
void show_all_consumers();

void dell_all_producers();
void dell_all_consumers();

void set_some_msgs();

void change_buffer_size(char sign);


////////////////////////////// VARIABLES ////////////////////////////// 

struct Message_queue_info msgs_info;

struct Producers_info producers;
struct Consumers_info consumers;


// Semaphores
sem_t *full_cond_sem;
sem_t *empty_cond_sem;
//sem_t *mutex_sem;
pthread_mutex_t mmute;



/// // MAIN // //
int main (int argc, char *argv[])
{

    datas_init();

/// - CHOICE PART - 
    char choice;
    show_menu();
    while (1) {
        printf("\nYou Can Enter Your Choice:\n");
        rewind(stdin);
        choice = getchar(); getchar();
 
        //printf("[it is {%c}]\n", choice);
        switch (choice) {
        case 'm' :
            show_menu();
            break; 
             
        /// Add Producer
        case 'p' : 
            if (producers.num == PRODUCERS_MAX_NUM) {
                fprintf(stderr, "\nMax number of Producers is Achieved\n");
            }
            else {
                add_producer();
            }
            break;

        /// Dellete Producer
        case 'd' : 
            if (producers.num > 0) {
                pthread_cancel(producers.pthreadd[producers.num - 1]);
                pthread_join(producers.pthreadd[producers.num - 1], NULL);
                producers.num--;
                printf("\nLast created Producer is successfully deleted. %d Producers left\n", producers.num);
            } else {
                fprintf(stderr, "\nNo Producers to delete\n");
            }
            break;

        /// Add Consumer
        case 'c' : 
            if (consumers.num == CONSUMERS_MAX_NUM) {
                fprintf(stderr, "\nMax number of Consumers is Achieved\n");
            }
            else {
                add_consumer();
            }
            break;

        /// Dellete Consumer
        case 'e' : 
            if (consumers.num > 0) {
                pthread_cancel(consumers.pthreadd[consumers.num - 1]);
                pthread_join(consumers.pthreadd[consumers.num - 1], NULL);
                consumers.num--;
                printf("\nLast created Consumer is successfully deleted. %d Consumers left\n", consumers.num);
            } else {
                fprintf(stderr, "\nNo Consumers to delete\n");
            }
            break;

        /// Get Statistic
        case 's' : 
            get_statistics();
            break;
        case 'l' : 
            show_all_messages();
            break; 

        /// Change Buff Size
        case '+' : 
            change_buffer_size('+');
            break;
        case '-' : 
            change_buffer_size('-');
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

/// Delete Child Proccesses
    dell_all_producers();
    dell_all_consumers();

    sem_close(full_cond_sem);
    sem_close(empty_cond_sem);
    //sem_close(mutex_sem);
    sem_unlink(FULL_COND_SEM_N);
    sem_unlink(EMPTY_COND_SEM_N);
    //sem_unlink(MUTEX_SEM_NAME);
    pthread_mutex_destroy(&mmute);

    free(msgs_info.msg_buff);

    exit(0);
}

/// ----------- Init datas -------------------------------------------------------------------------------------
void datas_init() {

    producers.num = 0;
    consumers.num = 0;

// Memory Datas Init
    msgs_info.added_msgs_counter = 1;
    msgs_info.delled_msgs_counter = 0;
    msgs_info.msgs_max_num = MSGS_STANDARD_NUM;
    msgs_info.msg_buff = (struct Message *)calloc(MSGS_STANDARD_NUM, sizeof(struct Message));
        if (msgs_info.msg_buff == NULL) {
            fprintf(stderr, "Init Allocation ERROR");
            exit(1);
        }

    // {0123......} {...0123...}{......0123}{23......01}
    msgs_info.head_ind = 0;
    msgs_info.tail_ind = 1;
    struct Message first_msg = { 'D', 346, 5, "!MMa!"};

    msgs_info.msg_buff[0] = first_msg;
    msgs_info.msgs_curr_num = 1;
    //set_some_msgs();

    /// Semaphores
    sem_unlink(FULL_COND_SEM_N);
    sem_unlink(EMPTY_COND_SEM_N);
    //sem_unlink(MUTEX_SEM_NAME);

    pthread_mutex_init(&mmute, NULL);
    
    full_cond_sem = sem_open(FULL_COND_SEM_N, O_CREAT, 0644, 1);
    empty_cond_sem = sem_open(EMPTY_COND_SEM_N, O_CREAT, 0644, 1);
    //mutex_sem = sem_open(MUTEX_SEM_NAME, O_CREAT, 0644, 1);

    if (full_cond_sem == SEM_FAILED || empty_cond_sem == SEM_FAILED) {
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
            "+ - Increase message buffer\n"
            "- - Decrease message buffer\n"
            "q - Quit \n");
}


/// ----------- Get Statistic ----------------------------------------------------------------------------------------
void get_statistics() {

    // Show queue stats
    show_queue_stats();

    // Show producers
    show_all_producers();
    // Show consumers
    show_all_consumers();

}

/// ----------- Show Queue stats -------------------------------------------------------------------------------------------------------
void show_queue_stats() {
    ///!!sem_wait(mutex_sem);

    printf("\n---Messages Buff Stats:---\n"
           "Number of Messages in buffer:%d\n"
           "Max Number of Messages:%d\n"
           "Number of Added messages:%d\n"
           "Number of Processed Messages:%d\n",
           msgs_info.msgs_curr_num, msgs_info.msgs_max_num, msgs_info.added_msgs_counter, msgs_info.delled_msgs_counter);

    ///!!sem_post(mutex_sem);
}


/// ----------- Show Messages ----------------------------------------------------------------------------------------
void show_all_messages() {

    pthread_mutex_lock(&mmute);
    // (*head == *tail)
    if (msgs_info.msgs_curr_num == 0) {
        printf("\nNo Messages in Buffer\n");
    }

    else {

        printf("\nHead&Tail inds:[%d][%d]\n", msgs_info.head_ind, msgs_info.tail_ind);


        for (int i = msgs_info.head_ind, k = 0; k < msgs_info.msgs_curr_num; k++) {
            printf("_MSG[%d]: {%d}{%d}{%d}{ ", i, msgs_info.msg_buff[i].ttype, 
                   msgs_info.msg_buff[i].hhash, msgs_info.msg_buff[i].ssize);
            for (int j = 0; j < msgs_info.msg_buff[i].ssize; j++) {
                printf(" %d ", msgs_info.msg_buff[i].ddata[j]);
            }
            printf(" }\n");

            i++;
            if (i == msgs_info.msgs_max_num) {
                i = 0;
            }
        } 
        

    }
    pthread_mutex_unlock(&mmute);
}


/// ----------- Show producers and consumers ----------------------------------------------------------------------------------------
void show_all_producers() {
    if (producers.num == 0) {
        printf("\n---No Working Producers---\n");
    }
    else {
        printf("\n---Working Producers:\n");
        for (int i = 0; i < producers.num; i++) {
            printf("Producer_%d Pthread: %ld;\n", i, producers.pthreadd[i]);
        }

    }
}

void show_all_consumers() {
    if (consumers.num == 0) {
        printf("\n---No Working Consumers---\n");
    }
    else {
        printf("\n---Working Consumers:\n");
        for (int i = 0; i < consumers.num; i++) {
            printf("Consumer_%d Pthread: %ld;\n", i, consumers.pthreadd[i]);
        }

    }
}





void set_some_msgs() {
    // {0123......} {...0123...}{......0123}{23......01}
    struct Message t_msg = { 'D', 346, 5, "!MMa!"};
    struct Message t_msg2 = { 'A', 3346, 4, "!jh!"};
    struct Message t_msg3 = { 'q', 306, 7, "CCHh!!c"};
    struct Message t_msg4 = { 'f', 32, 2, "Rd"};
    struct Message t_msg5 = { 'u', 45, 4, "!!MM"};

    // {0123......} 
    /*
    msgs_info.msg_buff[1] = t_msg2;
    msgs_info.msg_buff[2] = t_msg3;
    msgs_info.msg_buff[3] = t_msg4;
    msgs_info.msg_buff[4] = t_msg5;
    msgs_info.head_ind = 0;
    msgs_info.tail_ind = 5;  
    */

    // {...0123...}
    /*
    msgs_info.msg_buff[3] = t_msg;
    msgs_info.msg_buff[4] = t_msg2;
    msgs_info.msg_buff[5] = t_msg3;
    msgs_info.msg_buff[6] = t_msg4;
    msgs_info.msg_buff[7] = t_msg5;
    msgs_info.head_ind = 3;
    msgs_info.tail_ind = 8; 
    */

    // {......0123}
    /*
    msgs_info.msg_buff[8] = t_msg;
    msgs_info.msg_buff[9] = t_msg2;
    msgs_info.msg_buff[10] = t_msg3;
    msgs_info.msg_buff[11] = t_msg4;
    msgs_info.msg_buff[12] = t_msg5;
    msgs_info.head_ind = 8;
    msgs_info.tail_ind = 0; 
    */
     
    // {23......01}
    //*
    msgs_info.msg_buff[11] = t_msg;
    msgs_info.msg_buff[12] = t_msg2;
    msgs_info.msg_buff[0] = t_msg3;
    msgs_info.msg_buff[1] = t_msg4;
    msgs_info.msg_buff[2] = t_msg5;
    msgs_info.head_ind = 11;
    msgs_info.tail_ind = 3; 
    //*/

    msgs_info.msgs_curr_num = 5;
}


void change_buffer_size(char sign) {
    if (sign != '-' && sign != '+') {
        return;
    }

    pthread_mutex_lock(&mmute);

    if (msgs_info.msgs_max_num <= 1 && sign == '-') {
        printf("\nBuffer size cannot be smaller\n");
    }
    else {
        int new_buff_size;
            if (sign == '+') 
                new_buff_size = msgs_info.msgs_max_num + 1;
            else new_buff_size = msgs_info.msgs_max_num - 1;

        struct Message * old_msg_buff = msgs_info.msg_buff;
        msgs_info.msg_buff = (struct Message *)calloc(new_buff_size, sizeof(struct Message));
            if (msgs_info.msg_buff == NULL) {
                fprintf(stderr, "Change Buffer Size Allocation ERROR");
                exit(1);
            }


        int k = 0;
        for (int i = msgs_info.head_ind; k < msgs_info.msgs_curr_num && k < new_buff_size; k++) {

            msgs_info.msg_buff[k] = old_msg_buff[i];

            i++;
            if (i == msgs_info.msgs_max_num) {
                i = 0;
            }
        }

        msgs_info.head_ind = 0;
        if (k >= new_buff_size) {
            msgs_info.tail_ind = 0;
            msgs_info.msgs_curr_num = new_buff_size;
        }
        else {
            msgs_info.tail_ind = k;
        }

        free(old_msg_buff);


        msgs_info.msgs_max_num = new_buff_size;
        printf("\nBuffer size is ");
            if (sign == '+') 
                printf("in");
            else 
                printf("de");
        printf("creased. It's new MAX size:%d\n", msgs_info.msgs_max_num);
    }

    pthread_mutex_unlock(&mmute);
}


/// ----------- Add producer   ------------------------------------------------------------------------------ 
  
void add_producer() {
    
    int ret = pthread_create(&(producers.pthreadd[producers.num]), NULL, produce_proc, NULL);
    if (ret) {
        fprintf(stderr, "\nCreating Producer ERROR");
        exit(1);
    }

    printf("\nParent Created Producer_%ld_\n", producers.pthreadd[producers.num]);
    producers.num++;   
}
 
struct Message generate_msg(); 
int add_msg_to_queue(struct Message _msg);
 
void * produce_proc(void * arg) {
    
    
    struct Message generated_msg;

    //generated_msg = generate_msg(); 
            //printf("\n_MSG: {%d}{%d}{%d}{ ", generated_msg.ttype, 
            //       generated_msg.hhash, generated_msg.ssize);
            //for (int j = 0; j < generated_msg.ssize; j++) {
            //    printf(" %d ", generated_msg.ddata[j]);
            //}
            //printf(" }\n"); 

  
    while (1) {
        //printf("\nPthread\n");
        generated_msg = generate_msg();
        //printf("\nMSG is generated\n");

        //sem_getvalue(empty_cond_sem, &num);
        //printf("\n%s waits %d\n", argv[0], num);
        sem_wait(empty_cond_sem);

        pthread_mutex_lock(&mmute);
        int ret = add_msg_to_queue(generated_msg);
        pthread_mutex_unlock(&mmute);

        if (ret) {
            printf("\nPthread_%ld ADDED MSG\n", pthread_self());
        }
        sem_post(empty_cond_sem);
        
        sleep(4);
    }

    exit(0);
}

struct Message generate_msg() {

    struct Message generated_msg;
    int rand_num = 0;
    srand(time(NULL));

    do {
        //srand(time(NULL));
        rand_num = rand() % MSIZE;
    } while (rand_num == 0 || rand_num == MSIZE);
    //printf("___%d___", rand_num);

    generated_msg.ttype = msgs_info.added_msgs_counter;
    generated_msg.ssize = (unsigned char)rand_num;
    generated_msg.hhash = 0; 
    for (int i = 0; i < rand_num; i++) {
        unsigned char r_n = (unsigned char)(rand() % MSIZE);
        generated_msg.ddata[i] = r_n;
        generated_msg.hhash += r_n;
    }

    return generated_msg;
}

int add_msg_to_queue(struct Message _msg) {
    if (msgs_info.head_ind == msgs_info.tail_ind && msgs_info.msgs_curr_num != 0) {
        return 0;
    }

    msgs_info.msg_buff[msgs_info.tail_ind] = _msg;
    msgs_info.tail_ind++;
    if (msgs_info.tail_ind == msgs_info.msgs_max_num) {
        msgs_info.tail_ind = 0;
    }

    msgs_info.added_msgs_counter++;
    msgs_info.msgs_curr_num++;

    return 1;
}



/// ----------- Add consumer    ------------------------------------------------------------------------------

void add_consumer(){

    int ret = pthread_create(&(consumers.pthreadd[consumers.num]), NULL, consume_proc, NULL);
    if (ret) {
        fprintf(stderr, "\nCreating Consumer ERROR");
        exit(1);
    }

    printf("\nParent Created Consumer_%ld_\n", consumers.pthreadd[consumers.num]);
    consumers.num++; 
}

int process_msg(struct Message * _msg);

void * consume_proc(void * arg) {

    struct Message read_msg;
    while(1) {
        //printf("\nHi from consumer\n");
        sem_wait(full_cond_sem);

        pthread_mutex_lock(&mmute);
        int ret = process_msg(&read_msg);
        pthread_mutex_unlock(&mmute);

        if (ret) {
            printf("\nConsumer_%ld_ Processed MSG:\n{%d}{%d}{%d}{ ", pthread_self(), read_msg.ttype, 
                   read_msg.hhash, read_msg.ssize);
            for (int j = 0; j < read_msg.ssize; j++) {
                printf(" %d ", read_msg.ddata[j]);
            }
            printf(" }\n");
        //    printf("\nPthread_%ld ADDED MSG\n", pthread_self());
        }

        sem_post(full_cond_sem);

        sleep(4);
    }

    exit(0);
}

int process_msg(struct Message * _msg) {
    if (msgs_info.msgs_curr_num == 0) {
        return 0;
    }

    (*_msg) = msgs_info.msg_buff[msgs_info.head_ind];
    msgs_info.head_ind++;
    if (msgs_info.head_ind == msgs_info.msgs_max_num) {
        msgs_info.head_ind = 0;
    }

    msgs_info.delled_msgs_counter++;
    msgs_info.msgs_curr_num--;

    return 1;
}


/// ----------- Dell producers and consumers----------------------------------------------------------------------------------------
void dell_all_producers() {
    for (int i = 0; i < producers.num; i++) {
        //kill(producers.producers_pids[i], SIGUSR1);
        pthread_cancel(producers.pthreadd[producers.num - 1]);
        pthread_join(producers.pthreadd[producers.num - 1], NULL);
    }
}

void dell_all_consumers() {
    for (int i = 0; i < consumers.num; i++) {
        pthread_cancel(consumers.pthreadd[consumers.num - 1]);
        pthread_join(consumers.pthreadd[consumers.num - 1], NULL);
    }
}
