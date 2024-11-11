#ifndef HASH_OPTS
#define HASH_OPTS

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <langinfo.h>
#include <time.h>

#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/evp.h>

#define FILE_STR_READ_SYBMS 100
#define TIME_STR_LEN 30

#define FILE_NAME_LEN 4096 //100

#define LOG_PROJ_FILE_NAME "../build/logfile.txt"
//#define LOG_FILE_NAME "../build/logfile.txt"

int files_hash_compare(char* file_name1, char* file_name2);
void file_digest_update(EVP_MD_CTX *mdctx, char* fname);
unsigned check_file_presense(char* file_name);
void digest_update(EVP_MD_CTX *mdctx, char* mess);


void link_create (char* old_path, char* new_path, char* log_f_name);
void add_data_in_file(char* file_name, char* str_to_add);
void time_log(char* file_name);

#endif
