
#include "optionals.h"


/////////////////////////// FUNCTIONS  /////////////////////////////////////

unsigned str_input(char* str, long unsigned int str_size);


/// // MAIN // //
int main (int argc, char *argv[])
{

    printf("Welcome. It's client\n\n");
    
    int client_main_fd = socket_init(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sock_adr = {0};
    sock_adr.sin_family = AF_INET;
    sock_adr.sin_port = htons(STANDARD_PORT);
    inet_pton_init(AF_INET, STANDARD_IP_ADDRESS, &sock_adr.sin_addr);
    connect_fun(client_main_fd, (struct sockaddr *) &sock_adr, sizeof sock_adr);

    // Contacting
    char entry_buff [BUF_SIZE];
    ssize_t nread;

    int ret = 0;
    printf(" /");
    while (1) {
        printf(" > ");
        memset(entry_buff, 0, BUF_SIZE);
        ret = str_input(entry_buff, BUF_SIZE);
        if (ret == 0) {
            continue;
        }

        write(client_main_fd, entry_buff, strlen(entry_buff));

        if (strcmp(entry_buff, QUIT_OPT) == 0) {
            break;
        }

        memset(entry_buff, 0, BUF_SIZE);
        usleep(200);
        nread = read(client_main_fd, entry_buff, BUF_SIZE);
        nread += 0;
        printf("%s", entry_buff);
    }


    //write(client_main_fd, entry_buff, strlen(entry_buff));


    //nread = read(client_main_fd, buffer, BUF_SIZE);
    //if (nread == -1) {
    //    fprintf(stderr, "\nRead() Client ERROR\n");
    //    exit(EXIT_FAILURE);
    //} 
    //if (nread == 0) {
    //    fprintf(stderr, "\nEND OF FILE ACHIEVED by Read() Client\n");
    //}

    //write(STDOUT_FILENO, buffer, nread);



    close(client_main_fd);
    exit(0); 
}









/// String Input
unsigned str_input(char* str, long unsigned int str_size) {

    rewind(stdin);
    str[0] = '\0';

    char *result = fgets(str, str_size, stdin);
    if (result == NULL) {
        // Reading Error Handling
        fprintf(stderr, "Reading Buff Error\n");
        return 0;
    } else if (strlen(str) == str_size - 1 && str[str_size - 2] != '\n') {
        // Buffer Overflow Error Handling
        fprintf(stderr, "Too Long Input\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        return 0;
    } else {
        str[strlen(str) - 1] = '\0';   
    }

    if (str[0] == '\0') {
        str[0] = ' ';
        str[1] = '\0';
    }

    return 1;
}






//void connect_fun(int sockfd, const struct sockaddr * addr, socklen_t addrlen);
//void inet_pton_init(int af, const char * src, void * dst);

/*

void connect_fun(int sockfd, const struct sockaddr * addr, socklen_t addrlen) {

    int res = connect(sockfd, addr, addrlen);
    if (res == -1) {
        fprintf(stderr, "\nConnect() ERROR\n");
        exit(EXIT_FAILURE);
    }

}

void inet_pton_init(int af, const char * src, void * dst) {

    int res = inet_pton(af, src, dst);
    if (res == 0) {
        fprintf(stderr, "\nInet_pton(). Invalid network address\n");
        exit(EXIT_FAILURE);
    }
    if (res == -1) {
        fprintf(stderr, "\nInet_pton() ERROR\n");
        exit(EXIT_FAILURE);
    }
}

*/




    /*
    printf("Hi. It's client\n");
    
    int client_main_fd = socket_init(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sock_adr = {0};
    sock_adr.sin_family = AF_INET;
    sock_adr.sin_port = htons(STANDARD_PORT);
    inet_pton_init(AF_INET, STANDARD_IP_ADDRESS, &sock_adr.sin_addr);
    connect_fun(client_main_fd, (struct sockaddr *) &sock_adr, sizeof sock_adr);

    // Contacting
    char entry_buff [BUF_SIZE];

    int ret = 0;
    while (1) {
        memset(entry_buff, 0, BUF_SIZE);
        ret = str_input(entry_buff, BUF_SIZE);
        if (ret == 0) {
            continue;
        }



        if () {
        }
    }


    //str_input(entry_buff, BUF_SIZE);


    ////write(client_main_fd, "Hello! From Anasthetik\n", 24);
    //write(client_main_fd, entry_buff, strlen(entry_buff));


    ssize_t nread;
    char buffer [BUF_SIZE];
    nread = read(client_main_fd, buffer, BUF_SIZE);
    if (nread == -1) {
        fprintf(stderr, "\nRead() Client ERROR\n");
        exit(EXIT_FAILURE);
    } 
    if (nread == 0) {
        fprintf(stderr, "\nEND OF FILE ACHIEVED by Read() Client\n");
    }

    write(STDOUT_FILENO, buffer, nread);



    close(client_main_fd);
    exit(0); 
    */ 
