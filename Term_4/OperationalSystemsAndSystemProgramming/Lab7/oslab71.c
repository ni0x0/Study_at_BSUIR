
#include "optional.h"


/////////////////////////// FUNCTIONS  /////////////////////////////////////
void datas_init();
void show_menu();

void change_buffer_size(char sign);

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

////////////////////////////// VARIABLES ////////////////////////////// 

struct Message_queue_info msgsq_info;

struct Producers_info prods;
struct Consumers_info consums;

pthread_mutex_t mmute;
pthread_cond_t prod_cond;
pthread_cond_t consum_cond;


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
            if (prods.num == PRODUCERS_MAX_NUM) {
                fprintf(stderr, "\nMax number of Producers is Achieved\n");
            }
            else {
                add_producer();
            } 
            break;

        /// Dellete Producer
        case 'd' : 
            if (prods.num > 0) {   
                //pthread_cancel(prods.pthreadd[prods.num - 1]);
                prods.prodi_is_working[prods.num - 1] = 0;
                pthread_cond_broadcast(&prod_cond);
                pthread_kill(prods.pthreadd[prods.num - 1], SIGALRM);
                
                pthread_join(prods.pthreadd[prods.num - 1], NULL);
                prods.num--;
                printf("\nLast created Producer is successfully deleted. %d Producers left\n", prods.num);
            } else {
                fprintf(stderr, "\nNo Producers to delete\n");
            }
            break;

        /// Add Consumer
        case 'c' : 
            if (consums.num == CONSUMERS_MAX_NUM) {
                fprintf(stderr, "\nMax number of Consumers is Achieved\n");
            }
            else {
                add_consumer();
            }
            break;

        /// Dellete Consumer
        case 'e' : 
            if (consums.num > 0) {   
                consums.consi_is_working[consums.num - 1] = 0;
                pthread_cond_broadcast(&consum_cond);
                pthread_kill(consums.pthreadd[consums.num - 1], SIGALRM);
                
                pthread_join(consums.pthreadd[consums.num - 1], NULL);
                consums.num--;
                printf("\nLast created Consumer is successfully deleted. %d Consumers left\n", consums.num);
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

    pthread_cond_destroy(&consum_cond);
    pthread_cond_destroy(&prod_cond);
    pthread_mutex_destroy(&mmute);
    free(msgsq_info.msg_buff);

    exit(0);
}


/// ----------- Init datas -------------------------------------------------------------------------------------
void datas_init() {

    prods.num = 0;
    consums.num = 0;

// Memory Datas Init
    msgsq_info.added_msgs_counter = 1;
    msgsq_info.delled_msgs_counter = 0;
    msgsq_info.msgs_max_num = MSGS_STANDARD_NUM;
    msgsq_info.msg_buff = (struct Message *)calloc(MSGS_STANDARD_NUM, sizeof(struct Message));
        if (msgsq_info.msg_buff == NULL) {
            fprintf(stderr, "Init Allocation ERROR");
            exit(1);
        }

    // {0123......} {...0123...}{......0123}{23......01}
    msgsq_info.head_ind = 0;
    msgsq_info.tail_ind = 1;
    struct Message first_msg = { 'D', 346, 5, "!MMa!"};

    msgsq_info.msg_buff[0] = first_msg;
    msgsq_info.msgs_curr_num = 1;
    //msgs_num = 1;
    //set_some_msgs();

    /// Pthread variables 
    int res;  
    res = pthread_mutex_init(&mmute, NULL);
    if (res) {
        fprintf(stderr, "\nMutex Init ERROR\n");
        exit(1);
    }

    res = pthread_cond_init(&prod_cond, NULL);
    if (res != 0) {
        fprintf(stderr, "\nProducer Cond Init ERROR\n");
        exit(1);
    }
    res = pthread_cond_init(&consum_cond, NULL);
    if (res != 0) {
        fprintf(stderr, "\nConsumer Cond Init ERROR\n");
        exit(1);
    }
}


/// ----------- Show menu ----------------------------------------------------------------------------------------
void show_menu() {
    printf ("\nm - Show menu\n"
            "p - Create  producer \n"
            "c - Create consumer \n"
            "d - Delete producer \n"
            "e - Delete consumer \n"
            "s - Get statistic\n"
            "l - See Messages in buffer\n"
            "+ - Increase message buffer\n"
            "- - Decrease message buffer\n"
            "q - Quit \n");
}


void change_buffer_size(char sign) {
    if (sign != '-' && sign != '+') {
        return;
    }

    pthread_mutex_lock(&mmute);

    if (msgsq_info.msgs_max_num <= 1 && sign == '-') {
        printf("\nBuffer size cannot be smaller\n");
    }
    else {
        int new_buff_size;
            if (sign == '+') 
                new_buff_size = msgsq_info.msgs_max_num + 1;
            else new_buff_size = msgsq_info.msgs_max_num - 1;

        struct Message * old_msg_buff = msgsq_info.msg_buff;
        msgsq_info.msg_buff = (struct Message *)calloc(new_buff_size, sizeof(struct Message));
            if (msgsq_info.msg_buff == NULL) {
                fprintf(stderr, "Change Buffer Size Allocation ERROR");
                exit(1);
            }


        int k = 0;
        for (int i = msgsq_info.head_ind; k < msgsq_info.msgs_curr_num && k < new_buff_size; k++) {

            msgsq_info.msg_buff[k] = old_msg_buff[i];

            i++;
            if (i == msgsq_info.msgs_max_num) {
                i = 0;
            }
        }

        msgsq_info.head_ind = 0;
        if (k >= new_buff_size) {
            msgsq_info.tail_ind = 0;
            msgsq_info.msgs_curr_num = new_buff_size;
        }
        else {
            msgsq_info.tail_ind = k;
        }

        free(old_msg_buff);


        msgsq_info.msgs_max_num = new_buff_size;
        printf("\nBuffer size is ");
            if (sign == '+') {
                printf("in");
                pthread_cond_signal(&prod_cond); 
            } else 
                printf("de");
        printf("creased. It's new MAX size:%d\n", msgsq_info.msgs_max_num);
    }

    pthread_mutex_unlock(&mmute);
}


/// ----------- Get Statistic ----------------------------------------------------------------------------------------
void get_statistics() {

    show_queue_stats();
    show_all_producers();
    show_all_consumers();
}

/// ----------- Show Queue stats -------------------------------------------------------------------------------------------------------
void show_queue_stats() {
     pthread_mutex_lock(&mmute);

     printf("\n---Messages Buff Stats:---\n"
           "Number of Messages in buffer:%d\n"
           "Max Number of Messages:%d\n"
           "Number of Added messages:%d\n"
           "Number of Processed Messages:%d\n",
           msgsq_info.msgs_curr_num, msgsq_info.msgs_max_num, msgsq_info.added_msgs_counter, msgsq_info.delled_msgs_counter);

     pthread_mutex_unlock(&mmute);
}


/// ----------- Show Messages ----------------------------------------------------------------------------------------
void show_all_messages() {

    pthread_mutex_lock(&mmute);

    if (msgsq_info.msgs_curr_num == 0) {
        printf("\nNo Messages in Buffer\n");
    }

    else {

        printf("\nHead&Tail inds:[%d][%d]\n", msgsq_info.head_ind, msgsq_info.tail_ind);


        for (int i = msgsq_info.head_ind, k = 0; k < msgsq_info.msgs_curr_num; k++) {
            printf("_MSG[%d]: {%d}{%d}{%d}{ ", i, msgsq_info.msg_buff[i].ttype, 
                   msgsq_info.msg_buff[i].hhash, msgsq_info.msg_buff[i].ssize);
            for (int j = 0; j < msgsq_info.msg_buff[i].ssize; j++) {
                printf(" %d ", msgsq_info.msg_buff[i].ddata[j]);
            }
            printf(" }\n");

            i++;
            if (i == msgsq_info.msgs_max_num) {
                i = 0;
            }
        } 
        

    }
    pthread_mutex_unlock(&mmute);
}


/// ----------- Show producers and consumers ----------------------------------------------------------------------------------------
void show_all_producers() {
    if (prods.num == 0) {
        printf("\n---No Working Producers---\n");
    }
    else {
        printf("\n---Working Producers:\n");
        for (int i = 0; i < prods.num; i++) {
            printf("Producer_%d Pthread: %ld;\n", i, prods.pthreadd[i]);
            //printf("Producer_%d Pthread: %ld --- %d;\n", i, prods.pthreadd[i], prods.prodi_is_working[i]);
        }

    }
}

void show_all_consumers() {
    if (consums.num == 0) {
        printf("\n---No Working Consumers---\n");
    }
    else {
        printf("\n---Working Consumers:\n");
        for (int i = 0; i < consums.num; i++) {
            printf("Consumer_%d Pthread: %ld;\n", i, consums.pthreadd[i]);
        }

    }
}





/// ----------- Add producer   ------------------------------------------------------------------------------ 
  
void add_producer() {
    
    prods.prodi_is_working[prods.num] = 1;
    int ret = pthread_create(&(prods.pthreadd[prods.num]), NULL, produce_proc, &prods.num);
    if (ret) {
        fprintf(stderr, "\nCreating Producer ERROR");
        exit(1);
    }

    printf("\nParent Created %d Producer_%ld_\n", prods.num, prods.pthreadd[prods.num]);
    //prods.prodi_is_working[prods.num] = 1;
    usleep(200);
    prods.num++;   
}
 
struct Message generate_msg(); 
int add_msg_to_queue(struct Message _msg);
void signal_handling(int signum) {
    //printf("\nALLARM\n");
}
 
void * produce_proc(void * arg) {
    
    int mt_id = *((int*)arg);
    signal(SIGALRM, signal_handling);
    //printf("\nMy TID [%d]\n", mt_id);
    
    struct Message generated_msg;

    //generated_msg = generate_msg(); 
    //int ret = add_msg_to_queue(generated_msg);

    
    while (1) {
        if (prods.prodi_is_working[mt_id] == 0) {
            break;
        }

        generated_msg = generate_msg();

        pthread_mutex_lock(&mmute);
        //while (msgsq_info.msgs_curr_num >= msgsq_info.msgs_max_num) {
        if (msgsq_info.msgs_curr_num >= msgsq_info.msgs_max_num) {
            //printf("\nPthread_%d is waiting\n", mt_id);
            pthread_cond_wait(&prod_cond, &mmute);
        }

        if (prods.prodi_is_working[mt_id] == 1) {
            int ret = add_msg_to_queue(generated_msg);
            if (ret) {
                printf("\nProducer Pthread_%d ADDED MSG\n", mt_id);
                pthread_cond_signal(&consum_cond); 
            }
            //else printf("\n~Th_%d_ No place to put\n", mt_id);
        }

        //pthread_cond_signal(&consum_cond);
        pthread_mutex_unlock(&mmute);

        if (prods.prodi_is_working[mt_id] == 0) {
            break;
        }
        //usleep(4000000);
        sleep(4);
    }
    
    //printf("\nTID [%d] is exit\n", mt_id);
    return NULL;
    //exit(0);
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

    generated_msg.ttype = rand() % 10;
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
    if (msgsq_info.head_ind == msgsq_info.tail_ind && msgsq_info.msgs_curr_num != 0) {
        return 0;
    }

    msgsq_info.msg_buff[msgsq_info.tail_ind] = _msg;
    msgsq_info.tail_ind++;
    if (msgsq_info.tail_ind == msgsq_info.msgs_max_num) {
        msgsq_info.tail_ind = 0;
    }

    msgsq_info.added_msgs_counter++;
    msgsq_info.msgs_curr_num++;

    return 1;
}




/// ----------- Add consumer    ------------------------------------------------------------------------------

void add_consumer(){

    consums.consi_is_working[consums.num] = 1;
    int ret = pthread_create(&(consums.pthreadd[consums.num]), NULL, consume_proc, &consums.num);
    if (ret) {
        fprintf(stderr, "\nCreating Consumer ERROR");
        exit(1);
    }

    printf("\nParent Created Consumer_%ld_\n", consums.pthreadd[consums.num]);
    usleep(200);
    consums.num++; 
}  

int process_msg(struct Message * _msg);

void * consume_proc(void * arg) {

    int mt_id = *((int*)arg);
    signal(SIGALRM, signal_handling);
    struct Message read_msg;
    while(1) {
        if (consums.consi_is_working[mt_id] == 0) {
            break;
        }
        //printf("\nHi from consumer_%d_\n", mt_id);

        pthread_mutex_lock(&mmute);
        if (msgsq_info.msgs_curr_num == 0) {
            //printf("\nConsum Pthread_%d is waiting\n", mt_id);
            pthread_cond_wait(&consum_cond, &mmute);
        }

        int ret = 0;
        if (consums.consi_is_working[mt_id] == 1) {
                ret = process_msg(&read_msg);

                if (ret) {
                    printf("\nConsumer_%d_ Pthread Processed MSG:\n{%d}{%d}{%d}{ ", mt_id, read_msg.ttype, 
                           read_msg.hhash, read_msg.ssize);
                    for (int j = 0; j < read_msg.ssize; j++) {
                        printf(" %d ", read_msg.ddata[j]);
                    }
                    printf(" }\n");
                    pthread_cond_signal(&prod_cond); 
                }

        }

        pthread_mutex_unlock(&mmute);

        if (consums.consi_is_working[mt_id] == 0) {
            break;
        }
        sleep(4);
        if (consums.consi_is_working[mt_id] == 0) {
            break;
        }
    }

    //printf("\nCons TID [%d] is exit\n", mt_id);
    return NULL;
}

int process_msg(struct Message * _msg) {
    if (msgsq_info.msgs_curr_num == 0) {
        return 0;
    }

    (*_msg) = msgsq_info.msg_buff[msgsq_info.head_ind];
    msgsq_info.head_ind++;
    if (msgsq_info.head_ind == msgsq_info.msgs_max_num) {
        msgsq_info.head_ind = 0;
    }

    msgsq_info.delled_msgs_counter++;
    msgsq_info.msgs_curr_num--;

    return 1;
}





/// ---------- Dell producers and consumers----------------------------------------------------------------------------------------
void dell_all_producers() {
    for (int i = 0; i < prods.num; i++) {
        prods.prodi_is_working[i] = 0;
        pthread_cond_broadcast(&prod_cond);
        pthread_kill(prods.pthreadd[i], SIGALRM);

        pthread_join(prods.pthreadd[i], NULL);
        //prods.num--;
    }
}

void dell_all_consumers() {
    for (int i = 0; i < consums.num; i++) {
        consums.consi_is_working[i] = 0;
        pthread_cond_broadcast(&consum_cond);
        pthread_kill(consums.pthreadd[i], SIGALRM);

        pthread_join(consums.pthreadd[i], NULL);
    }
}
