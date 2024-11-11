#include "optionals.h"


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



// for client

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




int read_msg(int _fd, char* buffer, int buf_size) {

    int nread = read(_fd, buffer, buf_size);
    if (nread == -1) {
        fprintf(stderr, "\nRead() Client ERROR\n");
        exit(EXIT_FAILURE);
    } 
    if (nread == 0) {
        fprintf(stderr, "\nEND OF FILE ACHIEVED by Read() Client on fd %d\n", _fd);
    }
    return nread;
}
