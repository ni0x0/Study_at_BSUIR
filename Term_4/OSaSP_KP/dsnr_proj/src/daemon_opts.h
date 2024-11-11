#ifndef DAEMON_OPTS
#define DAEMON_OPTS

#include "database_opts.h"

#include <signal.h>


//#define DAEMON_STAT_FILE_NAME "daemon_stat.bin"
#define DAEMON_PID_FILE_NAME "daemon_stat.pid"


////////////////// FUNS //////////////////////////////

void daemon_start();

void daemon_create();


void sig_term_handler(int sign);

pid_t get_daemon_stat(char* f_n);
unsigned check_process_exists(pid_t pid);

void daemon_off();

void inot_events_proccessing ();

void in_write_closed_process(char* file_name);
void del_process(char* file_name);
void del_dir_process(char* dir_name);

#endif

