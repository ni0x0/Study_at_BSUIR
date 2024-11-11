#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char *argv[]) {

    fprintf(stdout, "Child process begins...\n");

    printf("Child pid: %d, ppid: %d\nChild command line arguments:\n", (int)getpid(), (int)getppid());
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }


    printf("Child environment arguments:\n");
    FILE *fp;
    fp = fopen(argv[1], "r");
    
    if (fp == NULL) {
        fprintf(stderr,"Error occured, error code  is %d\n", errno);
        exit(errno);
    }

    char file_str[20];

    while(!feof(fp)) {
        if (fscanf(fp, "%s", file_str) > 0) {
            printf("%s=%s\n", file_str, getenv(file_str));
        }
    }

    fclose(fp);

    printf("\n");
    exit(0);
}


