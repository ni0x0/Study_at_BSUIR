#ifndef OPTIONALS_H
#define OPTIONALS_H

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/wait.h>  
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <dirent.h>

#define MAX_CLIENTS_NUM 10
#define BUF_SIZE 10000 //6000 //2048 //1024 // 4096
#define STANDARD_IP_ADDRESS "127.0.0.1"
#define STANDARD_PORT 34543

#define DIR_NAME_LEN 4096
#define FILES_MSG_LEN 6000

#define QUIT_OPT "QUIT"
#define ECHO_OPT "ECHO"
#define INFO_OPT "INFO"
#define LIST_OPT "LIST"
#define CD_OPT "CD "



int socket_init(int domain, int typee, int protocol);
void bind_fun(int socket_fd, const struct sockaddr * addr, socklen_t addrlen); 
void listen_fun(int sock_fd, int backlog);
int accept_fun(int sockfd, struct sockaddr * addr, socklen_t * addrlen);

void connect_fun(int sockfd, const struct sockaddr * addr, socklen_t addrlen);
void inet_pton_init(int af, const char * src, void * dst);

int read_msg(int _fd, char* buffer, int buf_size);

#endif
