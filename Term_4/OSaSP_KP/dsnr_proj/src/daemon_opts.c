
#include "daemon_opts.h"

void daemon_start() {
    // Clear old datas
    clear_database(FILES_DATABASE_NAME, 1);
    ///clear_database(SUBDIRS_FOR_INOT_DATABASE_NAME, 
    clear_database(INOT_SUBDIRS_DATABASE_NAME, 0); 

    //  Save Datas

    char log_f_name [FILE_NAME_LEN] = LOG_PROJ_FILE_NAME;
    if (chosen_dirs_process(log_f_name) == 0) {
        printf ("NO DIRECTORIES TO MONITOR\n");
        return;
    }
    
    daemon_create();
    printf("Daemon is switched on\n");

    //
    //inot_close();
}


//char pid_file_name [FILE_NAME_LEN] = DAEMON_PID_FILE_NAME;
//char logirr_file_name [FILE_NAME_LEN] = LOG_PROJ_FILE_NAME;

/// Daemon Create Function
void daemon_create() {

    pid_t pid, sid; 
    // Fork off the parent process
    pid = fork();
    if (pid < 0) {
        // Error handling
        fprintf(stderr, "Daemon Creating ERROR");
        exit(1);
    }
    if (pid > 0) {
        // Parent proc code:
        //printf("\nDaemon is successfully created [%d]\n", pid);
        save_int_data(DAEMON_PID_FILE_NAME, pid);
        return;
    }
    else {
        // Child proc code:
        //printf("\nHi from child. Pid:[%d], ppid:[%d]", getpid(), getppid());

        // Create a new SID for the child process
        sid = setsid();
        if (sid < 0) {
            exit(EXIT_FAILURE);
        }      

        // Change the current working directory
        //if ((chdir("/")) < 0) {
        //    exit(EXIT_FAILURE);
        //}

        // Close out the standard file descriptors
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);


        signal(SIGTERM, sig_term_handler);

        //while (1) {
        // proc signals
        //}
        inot_events_proccessing ();

        exit(0);
    }
}



void sig_term_handler(int sign) {

    remove(DAEMON_PID_FILE_NAME);

    inot_close();
    remove(FD_INOT_FILE_NAME);
    exit(0);
}


void save_daemon_data(pid_t daem_pid) {
    FILE * stat_file = fopen(DAEMON_PID_FILE_NAME, "w");
    if (!stat_file) {
        fprintf(stderr, "Creating Daemon PID file.\n");
        exit(1);
    }

    fprintf(stat_file, "%d", daem_pid);
    fclose(stat_file);
}

/// Daemon data functions 
pid_t get_daemon_stat(char* f_n) {
    pid_t pid;
    //if ((pid = get_daemon_data(f_n)) == 0) { 
    if ((pid = get_int_data(f_n)) == 0) { 
        return 0; 
    }

    if (check_process_exists(pid) == 0) {
        return 0;
    }
    return pid;
}

pid_t get_daemon_data(char* f_n) {
    FILE * pid_file = fopen(f_n, "r");
    if (!pid_file) {
        return 0;
    }

    pid_t pid;
    fscanf(pid_file, "%d", &pid);
    fclose(pid_file);

    return pid;
}

unsigned check_process_exists(pid_t pid) {
    if (kill(pid, 0) == 0) {
        return 1;
    }
    else {
        return 0;
    }
}


/// Turn off daemon
void daemon_off() {
    pid_t pid = get_daemon_stat(DAEMON_PID_FILE_NAME);
    if (pid == 0) {
        //printf("No dae to del");
        return;
    }
    //printf("Dae to del[%d]", pid);

    kill(pid, SIGTERM);
}




/// Inot Events Proccessing
 
 
void inot_events_proccessing () {
    int length, i = 0;
    char buffer[BUF_LEN];

    // read events while start settings
    int fd = get_int_data(FD_INOT_FILE_NAME);
    length = read(fd, buffer, BUF_LEN);
    while (i < length) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];

        // next event
        i += EVENT_SIZE + event->len;
    }

    char dir_str[FILE_NAME_LEN];
    char wd_str[VALUE_STR_SIZE];    
    
    while (1) {

        i = 0;
        // read events data in buff
        length = read(fd, buffer, BUF_LEN);
        // check read data
        if (length < 0) {
            perror("read");
        }
        //add_data_in_file(LOG_PROJ_FILE_NAME, "\nAFTER READ");

        // process read data
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            
            sprintf(wd_str, "%d", event->wd);
            int gdfl;
            gdfl = get_data_by_key_in_db_tree (INOT_SUBDIRS_DATABASE_NAME, wd_str, dir_str, 0);
            if (gdfl == 0) {
                break;
            }
            //printf("\n In %s\n", dir_str);

            char f_name[FILE_NAME_LEN];
            strcpy(f_name, dir_str);
            strcat(f_name, event->name);

            //add_data_in_file(LOG_PROJ_FILE_NAME, "\n\n!!! ");
            //add_data_in_file(LOG_PROJ_FILE_NAME, f_name);
            if (event->mask & IN_ISDIR) {
                strcat(f_name, "/");
                if (event->mask & IN_CREATE) { 
                    //add_data_in_file(LOG_PROJ_FILE_NAME, " was write created\n\n"); 
                    dir_run_save_files_in_db (f_name, FILES_DATABASE_NAME, LOG_PROJ_FILE_NAME);
                }
                if (event->mask & IN_MOVED_FROM) {
                    //add_data_in_file(LOG_PROJ_FILE_NAME, " was moved from\n\n");

                    del_dir_process(f_name);
                    //break;
                }
                if (event->mask & IN_MOVED_TO) {
                    //add_data_in_file(LOG_PROJ_FILE_NAME, " was moved from\n\n");
                    dir_run_save_files_in_db (f_name, FILES_DATABASE_NAME, LOG_PROJ_FILE_NAME);
                }


                //if (event->mask & IN_DELETE) {
                    //add_data_in_file(LOG_PROJ_FILE_NAME, " dir was deleted\n\n");
                    
                    //del_dir_process();
                //}
            }
            else {
                if (event->mask & IN_CLOSE_WRITE) { 
                    //add_data_in_file(LOG_PROJ_FILE_NAME, " was write closed\n\n");
                    in_write_closed_process(f_name);
                }
                if (event->mask & IN_DELETE) {
                    //add_data_in_file(LOG_PROJ_FILE_NAME, " was deleted\n\n");
                    del_process(f_name);
                }
                if (event->mask & IN_MOVED_FROM) {
                    //add_data_in_file(LOG_PROJ_FILE_NAME, " was moved from\n\n");
                    if (event->name[0] == '.') {
                        break;
                    }
                    //add_data_in_file(LOG_PROJ_FILE_NAME, " was moved from\n\n");
                    del_process(f_name);
                }
                if (event->mask & IN_MOVED_TO) {
                    //add_data_in_file(LOG_PROJ_FILE_NAME, " was moved to\n\n");
                    in_write_closed_process(f_name);
                }
                if (event->mask & IN_MOVE_SELF) {
                    //int a = 9;
                    //a+=8;
                    del_dir_process(dir_str);
                }

            }

            // to next event in buff
            i += EVENT_SIZE + event->len;
        }


    }    
    
}



void in_write_closed_process(char* file_name) {
    sleep(2);
    struct stat s;
    if (lstat(file_name, &s) == -1) {
        return;
    }
    char size_str[VALUE_STR_SIZE];
    sprintf(size_str, "%ld", s.st_size);

    if (file_processing(&s, file_name, LOG_PROJ_FILE_NAME) == 0) {
        add_to_tree_database(FILES_DATABASE_NAME, file_name, size_str, 1);
    }  
}



void del_process(char* file_name) {
    del_data_db (FILES_DATABASE_NAME, file_name); 
}


void del_dir_process(char* dir_name) {

    del_dir_files_from_db (dir_name);
    del_dirs_wds(dir_name) ; 

}








/*


    int length, i = 0;
    //int fd;
    //int wd, wd2;
    char buffer[BUF_LEN];

length = read(fd, buffer, BUF_LEN);
while (i < length) {
    struct inotify_event *event = (struct inotify_event *) &buffer[i];

    // ßÕàÕåÞÔØÜ Ú áÛÕÔãîéÕÜã áÞÑëâØî Ò ÑãäÕàÕ
    i += EVENT_SIZE + event->len;
}


char dir_str[FILE_NAME_LEN];
char wd_str[VALUE_STR_SIZE];

    time_t start_time = time(NULL);
    time_t curr_time = start_time;


    while (curr_time - start_time < 40) {


        i = 0;
        // áçØâëÒÐÕÜ ÔÐÝÝëÕ Ò ÑãäÕà
        length = read(fd, buffer, BUF_LEN);

        // ßàÞÒÕàïÕÜ ãáßÕèÝÞáâì çâÕÝØï
        if (length < 0) {
            perror("read");
        }

        // ÞÑàÐÑÐâëÒÐÕÜ áÞÑëâØï Ò ÑãäÕàÕ
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            
            sprintf(wd_str, "%d", event->wd);
            get_data_by_key_in_db_tree (INOT_SUBDIRS_DATABASE_NAME, wd_str, dir_str, 0);
            printf("\n In %s\n", dir_str);


            if (event->mask & IN_ISDIR) {
                printf("\nThe directory %s ", event->name);
            }
            else 
                printf("\nThe file %s ", event->name);


            if (event->mask & IN_CLOSE_WRITE) {
                printf("was write close.\n");
            }
            if (event->mask & IN_CREATE) {
                printf("was created.\n");

                time_log(LOG_FILE_NAME);
                add_data_in_file(LOG_FILE_NAME, event->name);
                add_data_in_file(LOG_FILE_NAME, " was created\n");
            }
            if (event->mask & IN_DELETE) {
                printf("was deleted.\n");
            }
            if (event->mask & IN_DELETE_SELF) {
                printf("was deleted selff.\n");
            }

            if (event->mask & IN_MODIFY) {
                printf("was modifyd.\n");
            }
            if (event->mask & IN_MOVED_TO) {
                printf("was moved to.\n");
            }
            if (event->mask & IN_MOVED_FROM) {
                printf("was moved from.\n");
            }
            if (event->mask & IN_MOVE_SELF) {
                printf("was moved self.\n");
            }


            // ßÕàÕåÞÔØÜ Ú áÛÕÔãîéÕÜã áÞÑëâØî Ò ÑãäÕàÕ
            i += EVENT_SIZE + event->len;
        }







        curr_time = time(NULL);
    }
 */
