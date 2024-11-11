
#include "optionals.h"

/////////////////////////// FUNCTIONS  /////////////////////////////////////
void root_dir_init(int argc, char** argv);
void server_initialization();
int empty_spot_serching();

void sigint_sig_handling(int signum);

void * client_processing(void * arg);

void get_echo(char * buff, char * echo_msg, char * curr_dir);
void get_info(char * buff, char * curr_dir);
void get_list(char * buff, char * dir, char * curr_dir);
void get_cd(char * buff, char * cd_dir, char * dir);
void get_dog(char * buff, char * f_name, char * dir);



/////////////////////////// VARIABLES  //////////////////////////////////

char* root_directory;
struct stat file_stat;
int cd_offset;

pthread_t clients_threads [MAX_CLIENTS_NUM] = {0}; //{1, 1, 1, 0, 1, 0, 1, 0 }; // 
int curr_clients_num;

pthread_mutex_t mmute = PTHREAD_MUTEX_INITIALIZER;

struct Client_info {
    int pthr_id;
    int client_fd;
};

int server_fd;
struct sockaddr_in sock_adr = {0};

/// // MAIN // //
int main (int argc, char *argv[])
{
    root_dir_init(argc, argv); 
    printf("\"%s\" is good dir\n", root_directory);


//init server
    server_initialization();
    printf("\nServer started its work and waiting for clients . . . . . \n"); 

    //for (int i = 0; i < MAX_CLIENTS_NUM; i++) {
    //    printf(" %ld ", clients_threads[i]);
    //}
    //printf("\nEmpty place: [%d]\n", empty_spot_serching());



    //struct sockaddr_in client_adr;
    //socklen_t client_adrlen = sizeof client_adr;

    while (1) {
        //printf("aaa\n");

        struct sockaddr_in client_adr;
        socklen_t client_adrlen = sizeof client_adr;

        struct Client_info info_for_thread;
        info_for_thread.client_fd = accept_fun(server_fd, (struct sockaddr *) &client_adr, &client_adrlen); // accept client
        printf("\nAccepted connection from %s:%d\n", inet_ntoa(client_adr.sin_addr), ntohs(client_adr.sin_port));

                pthread_mutex_lock(&mmute);
        if (curr_clients_num < MAX_CLIENTS_NUM) {

            //int empty_pthread_spot = empty_spot_serching();
            //struct Client_info info_for_thread;
            info_for_thread.pthr_id = empty_spot_serching();

            pthread_create(&(clients_threads[info_for_thread.pthr_id]), NULL, client_processing, &info_for_thread);
            usleep(400);
            pthread_detach(clients_threads[info_for_thread.pthr_id]);
            curr_clients_num++;
                
            printf("Now server is serving %d clients\n", curr_clients_num);

        } else {
            printf("Max num of clients is achieved\n");
            //write(client_sock, "Server is busy, try again later.\n", strlen("Server is busy, try again later.\n"));
            //close(client_sock);
        }
                pthread_mutex_unlock(&mmute);
        
        sleep(2);
    }


    exit(0); 
}


/// Root Directory Initialization
void root_dir_init(int argc, char** argv) {

    printf("Hello. It is server configuration start.\n");
    if (argc != 2) {
        fprintf(stderr, "\nWrong server start. Startup should be:\"./server root_directory\".\n\n");
        exit(EXIT_FAILURE);
    }

    root_directory = argv[1];
    if (root_directory[strlen(root_directory) - 1] != '/') {
        strcat(root_directory, "/");
    }

    int res = lstat(root_directory, &file_stat);
    if (res == -1) {
        fprintf(stderr, "\nWrong file name.\n\n");
        exit(EXIT_FAILURE);
    }

    if ((file_stat.st_mode & S_IFMT) != S_IFDIR) {
        fprintf(stderr, "\nChosen file is not directory.\n\n");
        exit(EXIT_FAILURE);
    }

    cd_offset = strlen(root_directory) - 1;
}

/// Server Data Initialization
void server_initialization() {

    curr_clients_num = 0;
    signal(SIGINT, sigint_sig_handling);

    server_fd = socket_init(AF_INET, SOCK_STREAM, 0);

    //struct sockaddr_in sock_adr = {0};
    sock_adr.sin_family = AF_INET;
    sock_adr.sin_port = htons(STANDARD_PORT);
    bind_fun(server_fd, (struct sockaddr *) &sock_adr, sizeof sock_adr);  //choose protocol and port

    listen_fun(server_fd, MAX_CLIENTS_NUM);
}


/// Empty spot searching
int empty_spot_serching() {

    for (int i = 0; i < MAX_CLIENTS_NUM; i++) {
        if (clients_threads[i] == 0) {
            return i;
        }
    }
    return MAX_CLIENTS_NUM;
}

/// Interaption signal Handling
void sigint_sig_handling(int signum) {
    printf("\nSIGNAL INTERAPTION HANDLING\n");

    // sleep
    // close fd
    // join every thread

    sleep(1);
    close(server_fd);
    printf("Server successfully ends its work.\n");
    exit(0);
}


/// Client Processing
void * client_processing(void * arg) {

    struct Client_info my_info = *((struct Client_info*)arg);
    printf("\nClient_handler_%d_ started its work\n", my_info.pthr_id);

    char current_dir [DIR_NAME_LEN];
    strcpy(current_dir, root_directory);
    //int cd_offset = strlen(root_directory) - 1;

    char buffer [BUF_SIZE];
    char buffer_answer [BUF_SIZE];
    ssize_t nread; // number of read byte

    while (1) {

        memset(buffer, 0, BUF_SIZE);
        memset(buffer_answer, 0, BUF_SIZE);
        nread = read_msg(my_info.client_fd, buffer, BUF_SIZE);
        nread += 0;
        if (nread == 0) {
            break;
        }

        //write(STDOUT_FILENO, buffer, nread); // what we get from client
        printf("\nClient_handler_%d_ accepted:[%s]\n", my_info.pthr_id, buffer);

    // QUIT
        if (strcmp(buffer, QUIT_OPT) == 0) {
            break;
        }

    // ECHO
        if ((strncmp(buffer, ECHO_OPT, strlen(ECHO_OPT)) == 0) && ((buffer + strlen(ECHO_OPT))[0] == ' ' || (buffer + strlen(ECHO_OPT))[0] == '\0')) {

            get_echo(buffer_answer, buffer + strlen(ECHO_OPT), current_dir + cd_offset);

            write(my_info.client_fd, buffer_answer, strlen(buffer_answer) + 1);
            continue;
        }
       
    // INFO
        if (strncmp(buffer, INFO_OPT, strlen(INFO_OPT)) == 0) {

            get_info(buffer_answer, current_dir + cd_offset);

            write(my_info.client_fd, buffer_answer, strlen(buffer_answer) + 1);
            continue;
        }
 
    // LIST
        if (strncmp(buffer, LIST_OPT, strlen(LIST_OPT)) == 0) {

            get_list(buffer_answer, current_dir, current_dir + cd_offset);

            write(my_info.client_fd, buffer_answer, strlen(buffer_answer) + 1);
            continue;
        }

        
    // CD 
        if (strncmp(buffer, CD_OPT, strlen(CD_OPT)) == 0) {

            get_cd(buffer_answer, buffer + strlen(CD_OPT), current_dir);
     
            write(my_info.client_fd, buffer_answer, strlen(buffer_answer) + 1);
            continue;
        }
    // @
        if (buffer[0] == '@') {

            get_dog(buffer_answer, buffer + 1, current_dir);

            write(my_info.client_fd, buffer_answer, strlen(buffer_answer) + 1);
            continue;
        }


        strcpy(buffer_answer, "Wrong command.\n ");
        strcat(buffer_answer, (current_dir + cd_offset));
        write(my_info.client_fd, buffer_answer, strlen(buffer_answer) + 1);
    }

    



        pthread_mutex_lock(&mmute);
    curr_clients_num--;
    clients_threads[my_info.pthr_id] = 0;
        pthread_mutex_unlock(&mmute);

    sleep(2);
    close(my_info.client_fd);
    printf("\nClient_%d_ ended work with client\n", my_info.pthr_id);
    pthread_exit(NULL);
}

int check_empty_msg(char * msg) {
    for (int i = 0; msg[i] != '\0'; i++) {
        if (msg[i] != ' ' && msg[i] != '\n') {
            return 1;
        }
    }
    return 0;
}

/// Server Answers
void get_echo(char * buff, char * echo_msg, char * curr_dir) {

    //if (check_empty_msg(echo_msg) == 0) {
    //    strcpy(buff, "empty string");
    //}
    //else {//}

    if (echo_msg[0] == ' ') {
        strcpy(buff, echo_msg + 1);
    }
    
    strcat(buff, "\n ");
    strcat(buff, curr_dir);
}

void get_info(char * buff, char * curr_dir) {

    char* answer_msg = "You are working with the most lovely Server.\nHere are the avaible options:\n"
                       "ECHO - get message after ECHO\n"
                       "QUIT - leave server\n"
                       "INFO - information about server\n"
                       "CD - change current directory\n"
                       "LIST - get list of files from current directory\n"
                       "@file - perform commands from the file\n ";
    strcpy(buff, answer_msg);
    strcat(buff, curr_dir);
}

void change_cur_dir_for_lnk(char * curr_dir, char * full_link_orig_path) {
    strcpy(curr_dir, full_link_orig_path);
    int i = strlen(full_link_orig_path);
    while (full_link_orig_path[i] != '/') {
        i--;
    }
    curr_dir[++i] = '\0';
}

int check_link_link(char * full_link_orig_path, char * link_orig_path, char * orig_curr_dir, char * answer_buff) {

    printf("1:[%s]\n2:[%s]\n", orig_curr_dir,link_orig_path);
    char curr_dir [DIR_NAME_LEN];
    strcpy(curr_dir, orig_curr_dir);
    

    while (1) {
        strcpy(full_link_orig_path, curr_dir);
        strcat(full_link_orig_path, link_orig_path);
        printf("{%s}\n", full_link_orig_path);

        memset(link_orig_path, 0, sizeof(link_orig_path) - 1);
        ssize_t len;
        len = readlink(full_link_orig_path, link_orig_path, sizeof(link_orig_path) - 1);
        if (len == -1) {
            strcat(answer_buff, " link getting error");
            return -1;
        }
        link_orig_path[strlen(link_orig_path)] = '\0';

        change_cur_dir_for_lnk(curr_dir, full_link_orig_path);
        printf("||%s||\n", curr_dir);
        printf("::%s::\n", link_orig_path);
        strcpy(full_link_orig_path, curr_dir);
        strcat(full_link_orig_path, link_orig_path);
        printf("(%s)\n", full_link_orig_path);

        struct stat slnk;
        if (lstat(full_link_orig_path, &slnk) == -1) {
            strcat(answer_buff, " link origin file info getting error");
            return -1;
        }

        if (!S_ISLNK(slnk.st_mode)) {
            break;
        }
    }



    return 0;
} 


int get_link_name(char * link_path, char * answer_buff, char * curr_dir) {

    char full_link_orig_path [DIR_NAME_LEN];
    strcpy(full_link_orig_path, curr_dir);

    char link_orig_path [DIR_NAME_LEN];
    ssize_t len;

    len = readlink(link_path, link_orig_path, sizeof(link_orig_path) - 1);
    if (len == -1) {
        strcat(answer_buff, " link getting error");
        return -1;
    }
    link_orig_path[len] = '\0';

    strcat(full_link_orig_path, link_orig_path);

    struct stat slnk;
    if (lstat(full_link_orig_path, &slnk) == -1) {
        strcat(answer_buff, " link origin file info getting error");
        return -1;
    }

    if (S_ISLNK(slnk.st_mode)) {
        strcat(answer_buff, ">");
        //if (check_link_link(full_link_orig_path, link_orig_path, curr_dir, answer_buff) == -1) {
        //    return -1;
        //}
    }
    strcat(answer_buff, " ");
    strcat(answer_buff, link_orig_path);
    
    return 0;
}

int get_file_type(char * curr_dir, char * file_n, char * answer_buff) {

    //printf("[[%s%s]]\n", curr_dir, file_n);

    struct stat s;
    char full_f_name [DIR_NAME_LEN];
    strcpy(full_f_name, curr_dir);
    strcat(full_f_name, file_n);
    //printf("[[%s]]\n", full_f_name);

        if (lstat(full_f_name, &s) == -1) {
            return 0;
        }
        if (S_ISDIR(s.st_mode)) { 
            return 1;
        }

        if (S_ISLNK(s.st_mode)) {
            strcat(answer_buff, " -->");
            get_link_name(full_f_name, answer_buff, curr_dir);

            return 2;
        }



    return 0;
}


void get_list(char * buff, char * dir, char * curr_dir) {
    
    struct dirent** dirnt;
    int files_num;

    files_num = scandir(dir, &dirnt, 0, alphasort);
    if (files_num == -1) {
        strcpy(buff, "Access ERROR on directory \"");
        strcat(buff, curr_dir);
        strcpy(buff, "\"\n");
    }
    else {
        int ind = 0;
        while (ind < files_num) {
            if (strcmp(dirnt[ind]->d_name, ".") && strcmp(dirnt[ind]->d_name, "..")) {

                strcat(buff, dirnt[ind]->d_name);

                if (get_file_type(dir, dirnt[ind]->d_name, buff) == 1) {
                     strcat(buff, "/");
                }
                
                strcat(buff, "\n");
            }

            free(dirnt[ind]);
            ind++;
        }
        free(dirnt);
    }
    strcat(buff, " ");
    strcat(buff, curr_dir);
}


void find_prev_slash(char * str, int ind, char * pos) { 

    ind -= 2;
    while (str[ind] != '/') {
        ind --;
    }
    strcpy(str + ind + 1, pos + 3);

}

void check_prev_dir(char* new_dir) {
    char prev_dir [DIR_NAME_LEN];
    strcpy(prev_dir, new_dir);
    //printf("\n{%s}\n", new_dir);
    while (1) {

        //if (strncmp(prev_dir, root_directory, strlen(root_directory)) == 0) {
            //printf("\n{%s} like root\n", prev_dir);
        //    strcpy(new_dir, prev_dir);
        //    return;
        //}

    
    char* position = strstr(new_dir + strlen(root_directory), "..");
    int ind = position - new_dir;
    if (position == NULL) {
       // printf("\nNO .. NO\n");
        return;
    }
    strcpy(prev_dir + ind, position + 3);
    //printf("\n{%s}\n", prev_dir);

    if (strcmp(prev_dir, root_directory) == 0) {
        //printf("\n{%s} like root\n", prev_dir);
        strcpy(new_dir, prev_dir);
        return;
    }

    find_prev_slash(prev_dir, ind, position);
     //printf("\n{%s}\n", prev_dir);
     strcpy(new_dir, prev_dir);
    }
}



void get_cd(char * buff, char * cd_dir, char * dir) {
    char new_dir_name [DIR_NAME_LEN];
    strcpy(new_dir_name, "");
    strcpy(new_dir_name, dir);
    strcat(new_dir_name, cd_dir);
    //strcpy(buff, " ");
    strcpy(buff, new_dir_name + cd_offset);
    //printf("\n{%s}{%s}\n", new_dir_name, buff);

    
    struct stat fst;
    int res = lstat(new_dir_name, &fst);
    if (res == -1) {
        strcat(buff, " file access ERROR\n ");
    }
    else if ((fst.st_mode & S_IFMT) != S_IFDIR) {
                strcat(buff, " is not directory\n ");
    }
    else {
        if (new_dir_name[strlen(new_dir_name) - 1] != '/') {
            strcat(new_dir_name, "/");
        }

        check_prev_dir(new_dir_name);

        strcpy(dir, new_dir_name);
        strcpy(buff, " ");
    }

    strcat(buff,  dir + cd_offset);
}


void get_dog(char * buff, char * f_name, char * dir) {

    //printf("{%s}{%s}{%s}", buff, f_name, dir);
    
    char full_f_name [DIR_NAME_LEN];
    strcpy(full_f_name, dir);
    strcat(full_f_name, f_name);
    strcpy(buff, full_f_name + cd_offset);
    FILE * fp = fopen(full_f_name, "r");
    if (fp == NULL) {
        strcat(buff, " opening problem\n ");
        strcat(buff, dir + cd_offset);
        return;
    }

    strcat(buff, " is good\n ");
    strcat(buff, dir + cd_offset);
    strcat(buff, " > ");

    char command_str [BUF_SIZE];
    while (fgets(command_str, BUF_SIZE, fp)) {
            command_str[strlen(command_str) - 1] = '\0';
        printf("{%s}", command_str);
        strcat(buff, command_str);
        strcat(buff, "\n");
    // ECHO
        if ((strncmp(command_str, ECHO_OPT, strlen(ECHO_OPT)) == 0) && ((command_str + strlen(ECHO_OPT))[0] == ' ' || (command_str + strlen(ECHO_OPT))[0] == '\0')) {
     // if (strncmp(command_str, ECHO_OPT, strlen(ECHO_OPT)) == 0) {

            get_echo(buff + strlen(buff), command_str + strlen(ECHO_OPT), dir + cd_offset);
            continue;
        }
    // INFO
        if (strncmp(command_str, INFO_OPT, strlen(INFO_OPT)) == 0) {

            get_info(buff + strlen(buff), dir + cd_offset);
            continue;
        }
    // LIST
        if (strncmp(command_str, LIST_OPT, strlen(LIST_OPT)) == 0) {

            get_list(buff + strlen(buff), dir, dir + cd_offset);
            continue;
        }
    // CD 
        if (strncmp(command_str, CD_OPT, strlen(CD_OPT)) == 0) {

            get_cd(buff + strlen(buff), command_str + strlen(CD_OPT), dir);
            continue;
        }


    }
    
    fclose(fp); 
    
}




/*
/// Add files from dir to database
void dir_run_save_files_in_db (char* _dirN, char* db_name, char* log_f_name) {
     // Get files
     struct dirent** dirnt;
     int files_num;

     files_num = scandir(_dirN, &dirnt, 0, alphasort);
     //printf("f_num:%d!\n", files_num);
     
     if (files_num == -1) {
         //fprintf(stderr, "Directory Access Error\n");
         time_log(log_f_name);
         add_data_in_file(log_f_name, _dirN);
         add_data_in_file(log_f_name, "directory access ERROR\n");
     } else {

        //add_to_tree_database(SUBDIRS_FOR_INOT_DATABASE_NAME, "dir", _dirN, 0);
        int fd = get_int_data(FD_INOT_FILE_NAME);
        int wd = inotify_add_watch(fd, _dirN, INOT_FLAGS);
        //printf ("00WD:[%d], {%s}\n", wd, _dirN);
            char val_str[VALUE_STR_SIZE];
            sprintf(val_str, "%d", wd);
            add_to_tree_database(INOT_SUBDIRS_DATABASE_NAME, _dirN, val_str, 0);          

        int name_len = strlen(_dirN);
        
        // Directory check
        int ind = 0;
        while (ind < files_num) {

            // Get full file name
            strcpy(_dirN + name_len, dirnt[ind]->d_name);

            struct stat s;
            if (lstat(_dirN, &s) == -1) {
                fprintf(stderr, "lstat ERROR.\n");
                exit (-1);
            }
            //printf("{%s}\n", dirnt[ind]->d_name);
            char size_str[VALUE_STR_SIZE];
            sprintf(size_str, "%ld", s.st_size);
 
            // Add it to base
            if (strcmp(dirnt[ind]->d_name, ".") && strcmp(dirnt[ind]->d_name, "..")) {

                if (!(S_ISDIR(s.st_mode))) {  // if it is not directory we proccess

                    if (file_processing(&s, _dirN, log_f_name) == 0) {
                        add_to_tree_database(db_name, _dirN, size_str, 1);
                    }   
                } 
                else { // Jump in DIR
                    strcat(_dirN, "/");
                    //printf("%s - DIR\n",  _dirN);
                    //add_to_tree_database(INOT_SUBDIRS_DATABASE_NAME, size_str, _dirN, 0); wds right now
                    dir_run_save_files_in_db(_dirN, db_name, log_f_name);
                }
            }

            _dirN[name_len] = '\0';
            free(dirnt[ind]);
            ind++;
        } // all dir files checked


        free(dirnt);
     } // end of else (right directory)      
}


*/







/*
/// Client Processing
void * client_processing(void * arg) {

    struct Client_info my_info = *((struct Client_info*)arg);
    printf("\nClient_handler_%d_ starts its work\n", my_info.pthr_id);

    char buffer [BUF_SIZE];
    ssize_t nread; // number of read byte

    nread = read(my_info.client_fd, buffer, BUF_SIZE);
    if (nread == -1) {
        fprintf(stderr, "\nRead() Client ERROR\n");
        exit(EXIT_FAILURE);
    } 
    if (nread == 0) {
        fprintf(stderr, "\nEND OF FILE ACHIEVED by Read() Client_%d_\n", my_info.pthr_id);
    }

    //write(STDOUT_FILENO, buffer, nread); // what we get from client
    printf("\nClient_handler_%d_ accepted:[%s]", my_info.pthr_id, buffer);

    //write(client_fd, buffer, nread); // answer for client
    write(my_info.client_fd, buffer, nread);


    //printf("\nClient_%d_ starts its work\n", my_info.pthr_id);


    //while (i < 3) {

    //    printf("\nClient_%d_\n", my_info.pthr_id);

    //    i++;
    //    sleep(1);
    //}

        pthread_mutex_lock(&mmute);
    curr_clients_num--;
    clients_threads[my_info.pthr_id] = 0;
        pthread_mutex_unlock(&mmute);

    sleep(2);
    close(my_info.client_fd);
    printf("\nClient_%d_ ends its work\n", my_info.pthr_id);
    pthread_exit(NULL);
} 
*/ 






    /*
    int server_fd = socket_init(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sock_adr = {0};
    sock_adr.sin_family = AF_INET;
    sock_adr.sin_port = htons(34543);
    bind_fun(server_fd, (struct sockaddr *) &sock_adr, sizeof sock_adr);  //choose protocol and port

    listen_fun(server_fd, MAX_CLIENTS_NUM);

    socklen_t adrlen = sizeof sock_adr;
    int client_fd = accept_fun(server_fd, (struct sockaddr *) &sock_adr, &adrlen); // accept client

    // read client msg
    ssize_t nread; // number of read byte
    char buffer [BUF_SIZE];
    nread = read(client_fd, buffer, BUF_SIZE);
    if (nread == -1) {
        fprintf(stderr, "\nRead() Client ERROR\n");
        exit(EXIT_FAILURE);
    } 
    if (nread == 0) {
        fprintf(stderr, "\nEND OF FILE ACHIEVED by Read() Client\n");
    }

    write(STDOUT_FILENO, buffer, nread); // what we get from client

    write(client_fd, buffer, nread); // answer for client

    sleep(2);
    close(client_fd);
    close(server_fd);
    
    */ 


/*
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS_NUM 5
#define BUF_SIZE 1024 
*/

//int socket_init(int domain, int typee, int protocol);
//void bind_fun(int socket_fd, const struct sockaddr * addr, socklen_t addrlen); 
//void listen_fun(int sock_fd, int backlog);
//int accept_fun(int sockfd, struct sockaddr * addr, socklen_t * addrlen);

/*
int socket_init(int domain, int typee, int protocol) {
    int res = socket(domain, typee, protocol);
    if (res == -1) {
        fprintf(stderr, "\nSocket() ERROR\n");
        exit(EXIT_FAILURE);
    }
    return res;   // socket descriptor
}


void bind_fun(int socket_fd, const struct sockaddr * addr, socklen_t addrlen) {

    int res = bind(socket_fd, addr, addrlen);
    if (res == -1) {
        fprintf(stderr, "\nBind() ERROR\n");
        exit(EXIT_FAILURE);
    }
}

void listen_fun(int sock_fd, int backlog) {

    int res = listen(sock_fd, backlog);
    if (res == -1) {
        fprintf(stderr, "\nListen() ERROR\n");
        exit(EXIT_FAILURE);
    }
}

int accept_fun(int sockfd, struct sockaddr * addr, socklen_t * addrlen) {

    int res = accept(sockfd, addr, addrlen);
    if (res == -1) {
        fprintf(stderr, "\nAccept() ERROR\n");
        exit(EXIT_FAILURE);
    }
    return res;
}

*/
