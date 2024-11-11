#ifndef DATABASE_OPTS
#define DATABASE_OPTS

#include "hash_opts.h"

#include <fcntl.h>
#include <sys/types.h>
#include <libdb/db.h>

//#include <linux/inotify.h>
#include <sys/inotify.h>


#define	DIRS_DATABASE_NAME "../database/directories_database.db"
#define	FILES_DATABASE_NAME "../database/files_database.db"

#define	INOT_SUBDIRS_DATABASE_NAME "../database/inot_subdirectories_database.db"
#define FD_INOT_FILE_NAME "inot_fd"

//#define FILE_NAME_LEN 4096 //100
#define VALUE_STR_SIZE 19

#define EVENT_SIZE  (sizeof (struct inotify_event))
#define BUF_LEN     (1024 * (EVENT_SIZE + 16))
#define INOT_FLAGS ( IN_CLOSE_WRITE | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVED_TO | IN_MOVED_FROM | IN_MOVE_SELF)


typedef struct File_info_st {
    char name[FILE_NAME_LEN];
    char size[VALUE_STR_SIZE];
} File_info;

/// Work with Dir
void add_directory_for_monit();
unsigned str_input(char* str, long unsigned int str_size);
unsigned get_full_dir_path(char* dir_name);

unsigned check_key_presence_in_db_tree (char* db_name, char* db_key, unsigned dups_fl);
unsigned check_dirs_on_sub(char* dir_name);
void add_to_tree_database(char* db_name, char* db_data, char* db_key, unsigned dups_fl);
DB * open_tree_database(char* db_name, unsigned dups_fl);


void show_list_of_monitored_directories();
void print_db_tree_content(char* db_name, unsigned print_vals_fl, unsigned dups_fl);
u_long get_num_of_records(DB *dbp);

void delete_directory_for_monit();
int num_str_check(char* str, int num1, int num2);
DBC * get_cursor_by_num(DB *dbp, u_long num);


/// Files Proccessing at Daemon Start
unsigned chosen_dirs_process(char* log_f_name);
void clear_database(char* db_name, unsigned dups_fl);

void dir_run_save_files_in_db (char* _dir_n, char* db_name, char* log_f_name);
void save_int_data(char* f_name, int _int_n);
unsigned file_processing(struct stat * s, char* full_f_name, char* log_f_name);

void inot_close();
int get_int_data(char* f_name);

unsigned get_data_by_key_in_db_tree (char* db_name, char* db_key, char* db_data, unsigned dups_fl);


void del_dir_files_from_db (char * dir_name);
void del_data_db (char * db_name, char* data_name);
void del_dirs_wds (char * dir_name);


void inot();

#endif
