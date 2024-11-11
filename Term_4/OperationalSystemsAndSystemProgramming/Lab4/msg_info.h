////////////////  MESSAGE  //////////////////////////////////// 

#define MSIZE 180
#define DDATA_S ((MSIZE + 3)/4)*4
#define MSGS_MAX_NUM 13

#define PRODUCERS_MAX_NUM 13
#define CONSUMERS_MAX_NUM 13

// Message Struct 
struct message {
    char ttype;
    int hhash;
    unsigned char ssize;    
    unsigned char ddata [DDATA_S];
};

// Information about Queue of Massages  Struct
struct message_queue_info {
    //key_t key;
    struct message msg_buff [MSGS_MAX_NUM];

    //struct message *head_ptr;
    //struct message *tail_ptr;
    int head_ind;
    int tail_ind;

    int msgs_curr_num;
    int added_msgs_counter;
    int delled_msgs_counter;
};


void check_f();

///////////////////////// SEMAPHORE /////////////////////////////////// 

#define FULL_COND_SEM_N "/full_condit_sem"
#define EMPTY_COND_SEM_N "/empty_cond_sem"
#define MUTEX_SEM_NAME "/mutex_sem"

