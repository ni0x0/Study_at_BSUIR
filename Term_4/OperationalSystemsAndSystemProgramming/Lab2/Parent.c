//#define _POSIX_C_SOURCE
#define _GNU_SOURCE
//#define _BSD_SOURCE
//#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
//#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
//#include <dirent.h>
#include <errno.h>
//#include <getopt.h>
#include <sys/wait.h>

void environment_print(char* env[]);
char** get_child_envp(const char* file_n);
void set_process_name_num(char* name, int num);


// // // MAIN // // //
int main(int argc, char* argv[], char* envp[]) {

    printf("\nParent procces start.\n\n Command line arguments:\n");
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    // Print environment elements
    environment_print(envp);

    // Get child environment
    char** child_environ = get_child_envp(argv[1]);

    // Child process
    int process_num = 0;
    int child_status;

    char* child_name = NULL;
    char child_process_name[] = "child_XX";

    char** child_argv = (char**)calloc(3, sizeof(char*));
    child_argv[0] = child_process_name;
    child_argv[1] = argv[1];

    char choice;
    while (1) {
        printf("\nEnter option to do:");
        rewind(stdin);
        choice = getchar(); getchar();

        // Get child name
        switch (choice) 
        {
            case '+':
                printf("Option + : Get child name by getenv\n");
                child_name = getenv("CHILD_PATH");
                break;

            case '*':
                printf("Option * : Get child name by scanning environment\n");

                for (int i = 0; envp[i] != NULL; i++) {
                    char* child_in_env = strstr(envp[i], "CHILD_PATH");

                    if (child_in_env != NULL && child_in_env == envp[i]) {
                        child_name = envp[i] + 11;
                        break;
                    }
                }

                break;

            case '&':
                printf("Option & : Get child name by scanning extern environ\n");

                extern char** environ;
                for (int i = 0; environ[i] != NULL; i++) {
                    char* child_in_env = strstr(environ[i], "CHILD_PATH");

                    if (child_in_env != NULL && child_in_env == environ[i]) {
                        child_name = environ[i] + 11;
                        break;
                    }
                }

                break;

            default: 
                break;
        }

        if (choice == 'q') {
            break;
        }

        // Child process
        if (child_name) {
            printf("Got child name:%s\n", child_name);
            set_process_name_num(child_process_name, process_num);
            process_num++;
            pid_t pid = fork();                                                                          // create child process

            if (pid == -1) {
                printf("Error occured, error coode - %d\n", errno);
                exit(errno);
            }
            if (pid == 0) {
                printf("Child process created.\n\n");
                execve(child_name, child_argv, envp);
            }
            wait(&child_status);
            printf("Child process %s have ended with %d exit status\n", child_process_name, child_status); 
            
        }
        else {
            fprintf(stderr, "CHILD_PATH IS NOT DECLARED\n");
            exit(errno);
        }

    }

    free(child_environ);
    exit(0);
}

// Sort strings
void str_sort(int num, char** strs)
{
	int i, j, m;
	char* kk;
	for (i = 0; i < num - 1; ++i)    // array view
	{
		m = i;                                   // comparing index element
		kk = strs[i];                         // the element
		for (j = i + 1; j < num; ++j) // right of i elem elems view
		{
            if (strcmp(strs[j], kk) < 0) {
                m = j;                                   // remember what we need
                kk = strs[j];
            }
		}
		strs[m] = strs[i];                        // replacement
		strs[i] = kk;
	}
}

// Print Environment elements
void environment_print (char* env[]) {

    // Count env elems
    int envs_num = 0;
    for (; env[envs_num]; envs_num++);

    // Copy env elems
    char** environ_copy = (char**)calloc(envs_num, sizeof(char*));
    for (int i = 0; i < envs_num; i++) {
        environ_copy[i] = env[i];
    }
    // Print sort elems
    str_sort(envs_num, environ_copy);
    printf("\nYour environment elements:\n");
    for (int i = 0; i < envs_num; i++) {
        printf("%s\n", environ_copy[i]);
    }

    free(environ_copy);
}


char** get_child_envp (const char* file_n) {
    printf("\nFile Name: %s\n", file_n);
  
    FILE *fp;
    fp = fopen(file_n, "r");
    
    if (fp == NULL) {
        fprintf(stderr,"Error occured, error code - %d\n", errno);
        exit(errno);
    }

    char file_str[20];
    int envs_num = 0;
    while(!feof(fp)) {
        if (fscanf(fp, "%s", file_str) > 0) {
            envs_num++;
        }
    }

    // Get envp for child
    char** environ_copy = (char**)calloc(envs_num + 1, sizeof(char*));
    fseek(fp, 0, 0);
    for (int i = 0; i < envs_num; i++) {
        fscanf(fp, "%s", file_str);
        environ_copy[i] = getenv(file_str) - strlen(file_str) - 1;
    }

    fclose(fp);

    return environ_copy;
}


void set_process_name_num(char* name, int num) {
    name[6] = num / 10 + '0';
    name[7] = num % 10 + '0';
}







    /*
    int child_status;
    //char *args[] = {"pr0gramm", "parm_1", (char*)0};
    pid_t pid = fork();

    if (pid == -1) {
        printf("Error occured, error coode - %d\n", errno);
        exit(errno);
    }
    if (pid == 0) {
        printf("Child process created. Please, wait...\n");
        //execve("./child", args, NULL);
        execve("./child", argv, env);
    }

    wait(&child_status);
    printf("Child process have ended with %d exit status\n", child_status);
    //return 0;
*/
