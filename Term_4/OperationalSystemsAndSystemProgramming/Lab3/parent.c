#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_PROCS_NUM 14
int current_procs_num = 0;


struct proc_info {
    pid_t ch_pid;
    char outp_is_allowed;
};

struct proc_info child_procs_inf [MAX_PROCS_NUM];

struct sigaction output_permission_ask_sigaction;
struct sigaction alarm_sigact;

///////////// functions //////////////////////////////////////////////////////////////// 
void initialization(); 
void commands_print();
void sa_output_permission_ask_handler(int sign, siginfo_t *si, void *context);
void alarm_handler(int sign);

void add_child_process();
void show_all_processes();
void kill_all_proccess();

// // // MAIN // // //
int main(int argc, char* argv[]) {


    initialization(); 
    commands_print();

    char ch_enter[10];

    char choice;
    int num_choice;
    while (1) {

        printf("\nYou can enter the option to do:\n");
        rewind(stdin);
        ch_enter[0] = '\0';
        fgets(ch_enter, 10, stdin);

        //for (int i = 0; ch_enter[i]; i++) {
        //    printf("\n[%c]", ch_enter[i]);
        //}

        choice = ch_enter[0];

        if (choice == 'q') {
            kill_all_proccess();
            break;
        }
        if (ch_enter[1] == '\n') {
            num_choice = -1;
        } else {
            num_choice = atoi(ch_enter + 1);
        }
        //printf("\nOption:[%s][%d]", ch_enter, num_choice);
                
        switch (choice) 
        {
        // Add child
        case '+':
            add_child_process();
        break;
        // Delete last child
        case '-':
            if (current_procs_num > 0) {
                kill(child_procs_inf[current_procs_num - 1].ch_pid, SIGUSR1);
                current_procs_num--;
                printf("\nLast created child is successfully deleted. %d childs left\n", current_procs_num);
            } else {
                fprintf(stderr, "\nNo childs to delete\n");
            }
        break;
        // Show all processes;
        case 'l':
            show_all_processes();
            break;
        // Kill all processes;
        case 'k':
            kill_all_proccess();
        break;

         // Forbid num (all)  process(es) to show statistic
        case 's':
            
            if (num_choice == -1) {
                for (int i = 0; i < current_procs_num; i++) {
                    child_procs_inf[i].outp_is_allowed = 0;
                }
            }
            else if (num_choice < current_procs_num) {
                child_procs_inf[num_choice].outp_is_allowed = 0;
            }
            else {
                fprintf(stderr, "\nYour entered num is too big\n");
            } 
             
        break;

        // Allow num (all)  process(es) to show statistic
        case 'g':

            if (num_choice == -1) {
                for (int i = 0; i < current_procs_num; i++) {
                    child_procs_inf[i].outp_is_allowed = 1;
                }
            }
            else if (num_choice < current_procs_num) {
                child_procs_inf[num_choice].outp_is_allowed = 1;
            }
            else {
                fprintf(stderr, "\nYour entered num is too big\n");
            }

        break;

        // Get <num> statistic
        case 'p':
            if (num_choice == -1 || num_choice >= current_procs_num) {
                fprintf(stderr, "\nYour entered WRONG value\n");
            } else {
                // Forbid all processes to show statistic
                for (int i = 0; i < current_procs_num; i++) {
                    child_procs_inf[i].outp_is_allowed = 0;
                }

                // Send request to show stat
                union sigval sv;
                sv.sival_int = 2;
                sigqueue(child_procs_inf[num_choice].ch_pid, SIGUSR2, sv);

                alarm(5);
            }
        break;

        default: 
            break;
        } 
        
    }        // end of choice cycle

    exit(0);
}



void initialization() {

    output_permission_ask_sigaction.sa_flags = SA_SIGINFO | SA_RESTART;
    output_permission_ask_sigaction.sa_sigaction  =  sa_output_permission_ask_handler;   
    sigaction(SIGUSR2, &output_permission_ask_sigaction, NULL);

    alarm_sigact.sa_flags = SA_RESTART;
    alarm_sigact.sa_handler = alarm_handler;
    sigaction(SIGALRM, &alarm_sigact, NULL);
}

void commands_print() {
    printf("Options: \n");
    printf(" + - add child \n");
    printf(" - - delete last child \n");

    printf(" l - show all processes \n");
    printf(" k - kill all proccess\n");

    printf(" s - forbid all  processes to show statistic \n");
    printf(" g - allow all  processes to show statistic \n");
    printf(" s<num> - forbid <num>  process to show statistic \n");
    printf(" g<num> - allow <num>  process to show statistic \n");
    printf(" p<num> - get <num> statistic \n");

    printf(" q - exit \n");
}

int get_permission_status(pid_t  pid) {
    for (int i = 0; i < current_procs_num; i++) {
        if (child_procs_inf[i].ch_pid == pid) {
            return (int)child_procs_inf[i].outp_is_allowed;
        }
    }
    fprintf(stderr, "Got signal from pid that doesn't supposed to exist");
    exit(-1);
}

void sa_output_permission_ask_handler(int sign, siginfo_t *si, void *context) {
    // si->si_value.sival_int - pid
    //printf("\nparent get usr2 from %d!!!!!!!!!!!perm&%d& \n", si->si_value.sival_int, get_permission_status(si->si_value.sival_int));
        union sigval sv;
        // must answer 0 or 1
        sv.sival_int = get_permission_status(si->si_value.sival_int);
        sigqueue(si->si_value.sival_int, SIGUSR2, sv);
}


void add_child_process() {

    if (current_procs_num < MAX_PROCS_NUM) {

        pid_t pid = fork();
        child_procs_inf[current_procs_num].ch_pid = pid;

        if (pid == 0) {
            // Child proc code:
            char *child_argv[] = {"C_k", NULL}; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 
            //char child_argv[2][10] = {"C_k", NULL};
            char child_argv_name[10] = {"C_kk"};
            sprintf(child_argv_name+2, "%d", current_procs_num);
            child_argv[0] = child_argv_name;

            execve("./child", child_argv, NULL);
        } else if (pid > 0) {
            printf("\nParent created child number %d\n", current_procs_num);
            child_procs_inf[current_procs_num].outp_is_allowed = 1;
            current_procs_num++;

        } else {
            fprintf(stderr, "Child Creation ERROR\n");
            exit(-1);
        }

    } else {
        fprintf(stderr, "\nAmount of chids reached its MAXIMUM\n");
    }

}

void show_all_processes() {
    printf("\nNum of childs:%d\n", current_procs_num);
    for (int i = 0; i < current_procs_num; i++) {
        printf("Child[%d]_pid:%d. Output permission status:%d\n", i, child_procs_inf[i].ch_pid, child_procs_inf[i].outp_is_allowed);
    }
}

void kill_all_proccess() {
    for (int i = 0; i < current_procs_num; i++) {
        kill(child_procs_inf[i].ch_pid, SIGUSR1);
        printf("Child[%d] is killed\n", i);
    }
    current_procs_num = 0;
}

void alarm_handler(int sign){
    for (int i = 0; i < current_procs_num; i++) {
        child_procs_inf[i].outp_is_allowed = 1;
    }
    //printf("ALLLLLLLAAAAARM");
}


















