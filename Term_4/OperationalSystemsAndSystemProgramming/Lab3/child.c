#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

///////////// variables //////////////////////////////////////////////////////////////////
volatile struct two_words {
    int first;
    int second;
} combination;  

static struct two_words zero_comb = { 0, 0 }, 
                    one_comb = { 0, 1 }, 
                    two_comb = {1, 0}, 
                    three_comb = {1, 1};

volatile struct four_words {
    int zero;
    int one;
    int two;
    int three;
} combins_count;

// SIGACTION STRUCTURES
struct sigaction alarm_sigaction;
struct sigaction exit_sigaction;
struct sigaction output_permission_sigaction;

// FLAGS
volatile sig_atomic_t exit_fl = 0;
volatile sig_atomic_t alarm_fl = 0;
volatile sig_atomic_t output_is_allowed = 1;

// Else
char** argv_ptr; 

///////////// functions //////////////////////////////////////////////////////////////////
void parametres_initialization();
void statistic_print();

void sigalrm_handler(int sign);
void exitsig_handler(int sign);
//void output_permission_handler(int sign);
void sa_output_permission_handler(int sign, siginfo_t *si, void *context);


// // // MAIN // // //
int main (int argc, char *argv[]) {

    //signal(SIGALRM, sigalrm_handler);
   parametres_initialization();
   argv_ptr = argv;
   combination = zero_comb;

   // alarm_fl = 0;
   int cycle_counter  = 0;
   // -----main cycle-----------------
    while (1) {

        alarm(1);
        
        while (1) {
            combination = zero_comb;
            if (alarm_fl == 1) break;
            combination = one_comb;
            if (alarm_fl == 1) break;
            combination = two_comb;
            if (alarm_fl == 1) break;
            combination = three_comb;
            if (alarm_fl == 1) break;

                if (exit_fl) {
                    exit(0);
                }
        }
        alarm_fl = 0;
        cycle_counter++;

        // Send parent permission to output info
        if (cycle_counter % 17 == 0) {
            //kill(getppid(), SIGUSR2);   
            union sigval sv;
            sv.sival_int = getpid();
            //printf("\nsSSSSSend parent usr2\n");
            sigqueue(getppid(), SIGUSR2, sv);

            pause();
            if (output_is_allowed == 1) {
                //printf("\ngot permission\n");
                statistic_print();
            }
        }

    } // -----------end of main cycle

    exit(0);
}

//int counter = 0;


void sigalrm_handler(int sign) {
    alarm_fl = 1;
    //printf ("{%d,%d}\n", combination.first, combination.second);

    if (combination.first == 0 && combination.second == 0) {
        combins_count.zero++;
    }
    if (combination.first == 0 && combination.second == 1) {
        combins_count.one++;
    }
    if (combination.first == 1 && combination.second == 0) {
        combins_count.two++;
    }
    if (combination.first == 1 && combination.second == 1) {
        combins_count.three++;
    }

    //counter++;
    //if (counter % 6 == 0) {
    //    statistic_print();
    //    //printf ("\n0 - %d, 1 - %d, 2 - %d, 3 - %d\n", combins_count.zero, combins_count.one, combins_count.two, combins_count.three);
    //}
    //alarm(1);
}

void sa_output_permission_handler(int sign, siginfo_t *si, void *context) {

    // 0 - restrict; 1 - allowed; 2 - show stat right now
    output_is_allowed = si->si_value.sival_int;
    //printf("Coool handler get {%d}", output_is_allowed);  //si->si_value.sival_int);

    if (output_is_allowed == 2) {
        statistic_print();
    }
}

void parametres_initialization() {
    combins_count.zero = combins_count.one = combins_count.two = combins_count.three = 0;

    // signal handlers initialization
    alarm_sigaction.sa_handler = sigalrm_handler;
    sigaction(SIGALRM, &alarm_sigaction, NULL);

    exit_sigaction.sa_handler = exitsig_handler;
    sigaction(SIGUSR1, &exit_sigaction, NULL);

    //output_permission_sigaction.sa_handler = output_permission_handler;
    output_permission_sigaction.sa_flags = SA_SIGINFO;
    output_permission_sigaction.sa_sigaction  =  sa_output_permission_handler;   
    sigaction(SIGUSR2, &output_permission_sigaction, NULL);
}

void put_print(char* str){
    for (int i = 0; str[i]; i++) {
        fputc(str[i], stdout);
    }
}

void statistic_print() {
    char num_buff[10];

    char buff[120] = "\nPid:";                
    sprintf(num_buff, "%d", getpid());
    strcat(buff, num_buff);

    strcat(buff, ". Ppid:");
    sprintf(num_buff, "%d", getppid());
    strcat(buff, num_buff);

    strcat(buff, ". ");
    strcat(buff, argv_ptr[0]);
    strcat(buff, " combinations_statistic:\n{0,0} - ");
    sprintf(num_buff, "%d", combins_count.zero);
    strcat(buff, num_buff);

    strcat(buff, "; {0,1} - ");
    sprintf(num_buff, "%d", combins_count.one);
    strcat(buff, num_buff);

    strcat(buff, "; {1,0} - ");
    sprintf(num_buff, "%d", combins_count.two);
    strcat(buff, num_buff);

    strcat(buff, "; {1,1} - ");
    sprintf(num_buff, "%d", combins_count.three);
    strcat(buff, num_buff);
    strcat(buff, ".\n");

    //printf("[%ld]", strlen(buff));

    put_print(buff);
}

void exitsig_handler(int sign){
    //printf("\nI got email to ex\n");
    exit_fl = 1;
}

/*
void output_permission_handler(int sign){
    if (output_is_allowed) {
        output_is_allowed = 0;
    }
    else output_is_allowed = 1;
} 
*/ 

