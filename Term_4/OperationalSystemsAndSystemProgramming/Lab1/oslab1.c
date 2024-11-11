#define _POSIX_C_SOURCE
#define _GNU_SOURCE
//#define _BSD_SOURCE
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>


struct flags {
    bool l_flag;    // only symbol link flag
    bool d_flag;   // only directories flag 
    bool f_flag;    // only files flag
    bool s_flag;    // LC_COLLATE sort flag
}; 

void find_directName (char* dName, int argc, char ** argv);
void dir_run(char* _dN, struct flags _flgs);

// MAIN //
int main (int argc, char *argv[])
{
    struct flags dirFlags = {false, false, false, false};         // format flags  
    char dirFullName [255] = "./";                                      // directory name

    // Get directory name
    find_directName(dirFullName, argc, argv);
    if (dirFullName[strlen(dirFullName) - 1] != '/' ) 
        strcat(dirFullName, "/");
    printf("Start Directory Name:||%s||\n", dirFullName);
    
    // Set flags
    int res;
    while ((res = getopt(argc, argv, "ldfs")) != -1) {        // get flags
        switch (res) {
        case 'l': 
            dirFlags.l_flag = true;
            break;
        case 'd': 
            dirFlags.d_flag = true;
            break;
        case 'f': 
            dirFlags.f_flag = true;
            break;
        case 's': 
            dirFlags.s_flag = true;
            break;
        }
        //printf("%c", res);
    }

    if (dirFlags.l_flag == false && dirFlags.d_flag == false && dirFlags.f_flag == false) {
        dirFlags.l_flag = true;
        dirFlags.d_flag = true;
        dirFlags.f_flag = true;
    } 
    //printf("Flags:l-%d, d-%d, f-%d, s-%d\n", dirFlags.l_flag, dirFlags.d_flag, dirFlags.f_flag, dirFlags.s_flag);
 
    // Directory run process
    dir_run(dirFullName, dirFlags); 

    return 0;
}

// Command line Parsing
void find_directName (char* dName, int argc, char ** argv){
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            strcpy(dName, argv[i]);
            break;
        }
    }
}

void dir_run (char* _dN, struct flags _flgs) {
    // Get files
     struct dirent** dirnt;
     int files_num;

     files_num = scandir(_dN, &dirnt, 0, alphasort);

     //printf("f_num:%d!\n", files_num);
     
     if (files_num == -1) 
         fprintf(stderr, "Error: Wrong Directory Name\n");
     else {

         /*
        // Get number of directories without "." and ".."
        int dirs_num = 0;
        for (int i = 0; i < files_num; i++) {
            if (dirnt[i]->d_type == DT_DIR) 
            dirs_num++;
        }
        dirs_num -= 2;
        //printf("!dirs_num:%d!\n", dirs_num);  
         */ 

        int name_len = strlen(_dN);
        //printf("!len_num:%d!\n", name_len);  
         

        // Directory check
        int ind = 0;
        while (ind < files_num) {
             //printf("%s\n", dirnt[ind]->d_name);
             //ind++;

            // Get and print file and its type 
            if (strcmp(dirnt[ind]->d_name, ".") && strcmp(dirnt[ind]->d_name, "..")) {

                if (dirnt[ind]->d_type == DT_DIR && _flgs.d_flag) {
                     printf("\tDIR\t%s%s\n", _dN, dirnt[ind]->d_name);
                 }
                 else if (dirnt[ind]->d_type == DT_REG && _flgs.f_flag) {
                     printf("\tREG\t%s%s\n", _dN, dirnt[ind]->d_name);
                 }
                 else if (dirnt[ind]->d_type == DT_LNK && _flgs.l_flag){
                     printf("\tLNK\t%s%s\n", _dN, dirnt[ind]->d_name);
                 }
                 else if (_flgs.l_flag && _flgs.d_flag && _flgs.f_flag){
                     printf("\tOTH\t%s%s\n", _dN, dirnt[ind]->d_name);
                 }

            }  // end of file name print


            // Jump in DIR
            if (dirnt[ind]->d_type == DT_DIR && strcmp(dirnt[ind]->d_name, ".") && strcmp(dirnt[ind]->d_name, "..")) {
            //    if (_dN[strlen(_dN) - 1] != '/' ) 
            //        strcat(_dN, "/");
                strcat(_dN, dirnt[ind]->d_name);
                strcat(_dN, "/");
            //    //printf("\n\tENTER{%s}", _dN);
                 dir_run(_dN, _flgs);
                _dN[name_len] = '\0';
            //    printf("\n");
            }

            free(dirnt[ind]);
             ind++;

        } // all dir files checked


        free(dirnt);
     } // end of else (right directory)      
}


/*
void dir_run0(char* _dN, struct flags _flgs){
    // get files
     struct dirent** dirnt;
     int files_num;

     if (_flgs.s_flag) 
         files_num = scandir(_dN, &dirnt, 0, alphasort);
     else 
         files_num = scandir(_dN, &dirnt, 0, 0);
     
     //printf("\t!f_num:%d!\n", files_num);

     if (files_num == -1) 
         printf("Wrong Directory Name\n");

     // get directories without "." and ".."
     int dirs_num = 0;
     for (int i = 0; i < files_num; i++) 
         if (dirnt[i]->d_type == DT_DIR) 
             dirs_num++;
    dirs_num -= 2;
    //printf("!dirs_num:%d!\n", dirs_num);
         
     int name_len = strlen(_dN);
     //printf("!len_num:%d!\n", name_len);

     if (files_num > 0) {
         int i = 0;
         while (i < files_num) {
             //   if (dirnt[i]->d_type == DT_DIR) {

             // get type
             if (dirnt[i]->d_type == DT_DIR && _flgs.d_flag){
                 printf("\tDIR\t%s", _dN);
                 if (_dN[strlen(_dN) - 1] != '/') 
                     printf("/");
                 printf("%s\n", dirnt[i]->d_name);
             }
             else if (dirnt[i]->d_type == DT_REG && _flgs.f_flag){
                 printf("\tREG\t%s", _dN);
                 if (_dN[strlen(_dN) - 1] != '/') 
                     printf("/");
                 printf("%s\n", dirnt[i]->d_name);
             }
             else if (dirnt[i]->d_type == DT_LNK && _flgs.l_flag){
                 printf("\tLNK\t%s", _dN);
                 if (_dN[strlen(_dN) - 1] != '/') 
                     printf("/");
                 printf("%s\n", dirnt[i]->d_name);
             }
             else if (_flgs.l_flag && _flgs.d_flag && _flgs.f_flag){
                 printf("\tOTH\t%s", _dN);
                 if (_dN[strlen(_dN) - 1] != '/') 
                     printf("/");
                 printf("%s\n", dirnt[i]->d_name);
             }
             

             // in DIR
            if (dirnt[i]->d_type == DT_DIR && strcmp(dirnt[i]->d_name, ".") && strcmp(dirnt[i]->d_name, "..")) {
                if (_dN[strlen(_dN) - 1] != '/' ) 
                    strcat(_dN, "/");
                strcat(_dN, dirnt[i]->d_name);
                //printf("\n\tENTER{%s}", _dN);
                dir_run(_dN, _flgs);
                _dN[name_len] = '\0';
                printf("\n");
            }

             //}
             free(dirnt[i]);
             i++;
         }
     free(dirnt);
     } 
}
*/

























/*
void find_dirName(char** dName, int argc, char ** argv){
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            *dName = argv[i];
            break;
        }
    }
}
*/
 

/*
void find_directName(char* dName, int argc, char ** argv){
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            strcpy(dName, argv[i]);
            break;
        }
    }
}

void dir_run(char* _dN, struct flags _flgs){
    // get files
     struct dirent** dirnt;
     int files_num;

     if (_flgs.s_flag) 
         files_num = scandir(_dN, &dirnt, 0, alphasort);
     else 
         files_num = scandir(_dN, &dirnt, 0, 0);
     
     //printf("\t!f_num:%d!\n", files_num);

     if (files_num == -1) 
         printf("Wrong Directory Name\n");

     // get directories without "." and ".."
     int dirs_num = 0;
     for (int i = 0; i < files_num; i++) 
         if (dirnt[i]->d_type == DT_DIR) 
             dirs_num++;
    dirs_num -= 2;
    //printf("!dirs_num:%d!\n", dirs_num);
         
     int name_len = strlen(_dN);
     //printf("!len_num:%d!\n", name_len);

     if (files_num > 0) {
         int i = 0;
         while (i < files_num) {
             //   if (dirnt[i]->d_type == DT_DIR) {

             // get type
             if (dirnt[i]->d_type == DT_DIR && _flgs.d_flag){
                 printf("\tDIR\t%s", _dN);
                 if (_dN[strlen(_dN) - 1] != '/') 
                     printf("/");
                 printf("%s\n", dirnt[i]->d_name);
             }
             else if (dirnt[i]->d_type == DT_REG && _flgs.f_flag){
                 printf("\tREG\t%s", _dN);
                 if (_dN[strlen(_dN) - 1] != '/') 
                     printf("/");
                 printf("%s\n", dirnt[i]->d_name);
             }
             else if (dirnt[i]->d_type == DT_LNK && _flgs.l_flag){
                 printf("\tLNK\t%s", _dN);
                 if (_dN[strlen(_dN) - 1] != '/') 
                     printf("/");
                 printf("%s\n", dirnt[i]->d_name);
             }
             else if (_flgs.l_flag && _flgs.d_flag && _flgs.f_flag){
                 printf("\tOTH\t%s", _dN);
                 if (_dN[strlen(_dN) - 1] != '/') 
                     printf("/");
                 printf("%s\n", dirnt[i]->d_name);
             }
             

             // in DIR
            if (dirnt[i]->d_type == DT_DIR && strcmp(dirnt[i]->d_name, ".") && strcmp(dirnt[i]->d_name, "..")) {
                if (_dN[strlen(_dN) - 1] != '/' ) 
                    strcat(_dN, "/");
                strcat(_dN, dirnt[i]->d_name);
                //printf("\n\tENTER{%s}", _dN);
                dir_run(_dN, _flgs);
                _dN[name_len] = '\0';
                printf("\n");
            }

             //}
             free(dirnt[i]);
             i++;
         }
     free(dirnt);
     } 
}

// MAIN //
int main (int argc, char *argv[])
{
    struct flags dirFlags = {false, false, false, false};         // format flags
    //char* dirName = "./";                                                     // directory name
    char dirFullName [255] = "./";

    // search directory name
    //printf("|%s|\n", dirName);
    //find_dirName(&dirName, argc, argv);
    find_directName(dirFullName, argc, argv);

    // set flags
    int res;
    while ((res = getopt(argc, argv, "ldfs")) != -1) {        // get flags
        switch (res) {
        case 'l': 
            dirFlags.l_flag = true;
            break;
        case 'd': 
            dirFlags.d_flag = true;
            break;
        case 'f': 
            dirFlags.f_flag = true;
            break;
        case 's': 
            dirFlags.s_flag = true;
            break;
        }
        //printf("%c", res);
    }

    if (dirFlags.l_flag == false && dirFlags.d_flag == false && dirFlags.f_flag == false) {
        dirFlags.l_flag = true;
        dirFlags.d_flag = true;
        dirFlags.f_flag = true;
    }
    

    printf("\nStart Directory Name:|%s|\n", dirFullName);
    //printf("%d%d%d%d\n", dirFlags.l_flag, dirFlags.d_flag, dirFlags.f_flag, dirFlags.f_flag);

    //char dirFullName [255];
    //strcpy(dirFullName, dirName); 
    //printf("|%s|\n", dirFullName);

    dir_run(dirFullName, dirFlags); 

    return 0;
}


*/

























/*
             // get type
             if (dirnt[i]->d_type == DT_DIR)
                 printf("\tDIR");
             else if (dirnt[i]->d_type == DT_REG)
                 printf("\tREG");
             else if (dirnt[i]->d_type == DT_LNK)
                 printf("\tLNK");
             else 
                 printf("\tOTH");
             
             // print name
             //if (_dN[0] == '.' && _dN[1] != '.' ) 
             if (_dN[strlen(_dN) - 1] == '/' ) 
                 printf("\t%s%s\n", _dN, dirnt[i]->d_name);
             else 
                 printf("\t%s/%s\n", _dN, dirnt[i]->d_name);
*/

