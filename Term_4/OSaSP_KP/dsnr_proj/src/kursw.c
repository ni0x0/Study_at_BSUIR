
#include "daemon_opts.h"

////////////////// FUNCTIONS /////////////////////////////
void show_menu();

void show_daemon_cond();
void turn_on_off_daemon();

void add_directory();
void del_directory();


////////////////// MAIN /////////////////////////////
int main (int argc, char *argv[])
{

// - CHOICE PART -
    char choice;
    show_menu();
    while (1) {
        printf("\nYour Choice:\n");
        rewind(stdin);
        choice = '-'; choice = getchar(); //getchar();

        if (getchar() != '\n') {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            printf("\nChoose option from the List and press Enter.\n");
            continue;
        }
        if (choice == 'q') {
            break;
        }  

        //printf("it is {%c}\n", choice);
        switch (choice) {
        case 'c' : 
            show_daemon_cond();
            break;       

        case 't' :                                        
            turn_on_off_daemon();
            break;

        case 'l' :
            show_list_of_monitored_directories();
            break;
        case 'a' :
            add_directory();
            break;
        case 'd' :
            del_directory();
            break; 


        case 'f' :
            print_db_tree_content(FILES_DATABASE_NAME, 1, 1);
            break;         
        case 's' :
            print_db_tree_content(INOT_SUBDIRS_DATABASE_NAME, 1, 0);
            break;

        default:
            printf("\nChoose option from the List and press Enter.\n");
            break;
        }
       

    } // end of chose cycle

    exit(0);
}



// Menu's Print
void show_menu() {

    printf("~~Duplicates Searcher & Replacer~~\n"
           "c - DSNR Daemon condition\n"
           "t - Turn On/Off DSNR Daemon\n\n"
           "l - List of Monitored Directories\n"
           "a - Add Directory for monitoring\n"
           "d - Delete Directory for monitoring\n\n"
           "q - Quit");

            // f - see files in database, s - subdirs
}

void show_daemon_cond() {

    pid_t d_pid = get_daemon_stat(DAEMON_PID_FILE_NAME); 
    if (d_pid == 0) {
        printf("Daemon is off\n");
    }
    else {
        printf("Daemon is on\nIt's pid:%d\n", d_pid);
    }
}

void turn_on_off_daemon() {

    pid_t d_pid = get_daemon_stat(DAEMON_PID_FILE_NAME); 
    if (d_pid == 0) {
        daemon_start();
    }
    else {
        daemon_off();
        printf("Daemon is switched off\n");
    }
}

void add_directory() {

    pid_t d_pid = get_daemon_stat(DAEMON_PID_FILE_NAME); 
    if (d_pid == 0) {
        //printf("Daemon is off\n");
        add_directory_for_monit();
    }
    else {
        //printf("Daemon is on\nIt's pid:%d\n", d_pid);
        daemon_off();
        add_directory_for_monit();
        daemon_start();
    }

}

void del_directory() {

    pid_t d_pid = get_daemon_stat(DAEMON_PID_FILE_NAME); 
    if (d_pid == 0) {
        //printf("Daemon is off\n");
        delete_directory_for_monit();
    }
    else {
        //printf("Daemon is on\nIt's pid:%d\n", d_pid);
        daemon_off();
        delete_directory_for_monit();
        daemon_start();
    }

}

