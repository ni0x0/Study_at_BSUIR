#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>  
#include <string.h>
#include <fcntl.h>
#include <malloc.h>

#define STUD_NAME_LEN 80
#define STUD_ADDRESS_LEN 80

struct File_record {
    char name [STUD_NAME_LEN];
    char address [STUD_ADDRESS_LEN];
    unsigned int semester;
};
#define STUDS_DATA_FILE_NAME "students_datas.bin"
#define STUDS_NUM 12


/////////////////////////// FUNCTIONS  /////////////////////////////////////
void show_menu();
void show_list_of_datas_from_file();
void add_studs_datas();
void choose_stud_data();
void choosen_data_info(); 
void modify_stud_data();
void save_mod_data();

////////////////////////////// VARIABLES ////////////////////////////// 

struct File_record chosen_student;
struct File_record old_chosen_student_info;
unsigned int chosen_student_index;
unsigned int chosen_student_is_chosened;
unsigned int chosen_student_is_modified;


/// // MAIN // //
int main (int argc, char *argv[])
{

    chosen_student_is_chosened = 0;
    chosen_student_is_modified = 0;

/// - CHOICE PART - 
    char choice;
    show_menu();
    while (1) {
        printf("\nYou Can Enter Your Choice:\n");
        rewind(stdin);
        choice = getchar(); getchar();
 
        //printf("[it is {%c}]\n", choice);
        switch (choice) {
        case 's' :
            show_menu();
            break; 

        case 'l' :
            show_list_of_datas_from_file();
            break; 

        case 'a' :
            add_studs_datas();
            break;

        case 'c' :
            choose_stud_data(); 
            break; 

        case 'i' :
            choosen_data_info(); 
            break; 

        case 'm' :
            modify_stud_data();
            break; 

        case 'd' :
            save_mod_data();
            break; 

        case '\n' : 
            getchar();
            break;

        default:
            break;
        }


        if (choice == 'q') {
            break;
        }  
    } // end of main while

    exit(0);
}



/// ----------- Show menu ----------------------------------------------------------------------------------------
void show_menu() {
    printf ("\ns - Show menu\n"
            "l - Get List of Datas from file \n"
            "a - Add Datas to file \n"
            "c - Choose Data \n"
            "i - Information about Chosen Student \n"
            "m - Modify Chosen Data \n"
            "d - Save Modified Data \n"
            "q - Quit \n");
}

int get_num_of_records();

/// ----------- Show list of datas    ----------------------------------------------------------------------------------------
void show_list_of_datas_from_file() {

    int num_of_records_in_f = get_num_of_records();
    if (num_of_records_in_f == 0) {
        return;
    }
   
    struct File_record stud_info;
    //FILE * fp = fopen(STUDS_DATA_FILE_NAME, "rb");
    int fd = open(STUDS_DATA_FILE_NAME, O_RDONLY, 0644);
    if (fd == -1) {
        fprintf(stderr, "\nFile \"%s\" Opening ERROR\n", STUDS_DATA_FILE_NAME);
        return;
    }

    //printf("\nNum of records in file \"%s\" is %d \n", STUDS_DATA_FILE_NAME, num_of_records_in_f);
    printf("\nList of records in file \"%s\"\n", STUDS_DATA_FILE_NAME);
    for (int i = 1; i <= num_of_records_in_f; i++) {
        //fread(&stud_info, sizeof(struct File_record), 1, fp);
        read(fd, &stud_info, sizeof(struct File_record));
        printf("%d -- Name: \"%s\" | Addres: \"%s\" | Semester: \"%d\"\n", i, stud_info.name, stud_info.address, stud_info.semester);
    }

    //fclose(fp);
    close(fd);
}



/// ----------- Add students datas    -------------------------------------------------------------------------------------
void add_studs_datas() {

    //FILE * fp = fopen(STUDS_DATA_FILE_NAME, "a+b");
    int fd = open(STUDS_DATA_FILE_NAME, O_RDWR | O_CREAT | O_APPEND, 0644);

    //if (fp == NULL) {
    if (fd == -1) {
        fprintf(stderr, "\nFile Opening ERROR\n");
        exit(1);
    }

    struct File_record stud_info [STUDS_NUM] = {
                                                 { "Charnavok Anastasia", "Minsk", 4 }, { "Arendaruk Vladislav", "Riga", 8 },
                                                 { "Gumnenkova Maria", "Vitebsk", 4 }, { "Maskalev Alexey", "Minsk", 6 },
                                                 { "Shulia Danila", "City Minsk", 4 }, { "Dorovko Yaroslav", "Grodno", 3 },
                                                 { "Nikitenko Anastasia", "Polotsk", 4 }, { "Ososhnik Anastasia", "Vitebsk", 6 },
                                                 { "Kapustina Maria", "Vitebsk", 8 }, { "Ogurchenok Kirill", "Vitebsk", 5 },
                                                 { "Yukhnevich Hanna", "Moscow", 4 }, { "Starikhov Zachar", "Gorodok", 4 } 
    };

    //fwrite(&stud_info, sizeof(struct File_record), STUDS_NUM , fp);
    write(fd, stud_info, sizeof(stud_info));

    //int bytes_in_f = ftell(fp);
    int bytes_in_f = lseek(fd, 0, SEEK_CUR);
    bytes_in_f /= sizeof(struct File_record);
    printf("Datas are successfully added. Current num of records in file is %d \n", bytes_in_f);

    //fclose(fp);
    close(fd);
}


unsigned str_input(char* str, long unsigned int str_size);

/// ----------- Choose data    ----------------------------------------------------------------------------------------
void choose_stud_data() {

    int num_of_records_in_f = get_num_of_records();
    if (num_of_records_in_f == 0) {
        return;
    }

    printf("\nNum of records in file \"%s\" is %d.\nEnter index of needed record: ", STUDS_DATA_FILE_NAME, num_of_records_in_f);
    char choice_str [10];
    if (str_input(choice_str, 10) == 0) {
        return;
    }
    //printf("\nYour str \"%s\"\n", choice_str);
    int choice_ind = atoi(choice_str);
    if (choice_ind == 0 || choice_ind > num_of_records_in_f) {
        fprintf(stderr, "\nWrong Index Value\n");
        return;
    }


    //FILE * fp = fopen(STUDS_DATA_FILE_NAME, "rb");
    int fd = open(STUDS_DATA_FILE_NAME, O_RDONLY, 0644);
    //if (fp == NULL) {
    if (fd == -1) {
        fprintf(stderr, "\nFile \"%s\" Opening ERROR\n", STUDS_DATA_FILE_NAME);
        return;
    }

    chosen_student_index = choice_ind - 1;
    //fseek(fp, chosen_student_index * sizeof(struct File_record), SEEK_SET);
    lseek(fd, chosen_student_index * sizeof(struct File_record), SEEK_SET);
    //fread(&chosen_student, sizeof(struct File_record), 1, fp);
    read(fd, &chosen_student, sizeof(struct File_record));

    old_chosen_student_info = chosen_student;
    chosen_student_is_chosened = 1;
    chosen_student_is_modified = 0;

    printf("\nChosen data for modifying:\nStud_%d: Name: \"%s\" | Addres: \"%s\" | Semester: \"%d\"\n", chosen_student_index + 1, chosen_student.name, chosen_student.address, chosen_student.semester);

    //fclose(fp);
    close(fd);
}

int get_num_of_records() {
    //FILE * fp = fopen(STUDS_DATA_FILE_NAME, "rb");
    int fd = open(STUDS_DATA_FILE_NAME, O_RDONLY, 0644);
    //if (fp == NULL) {
    if (fd == -1) {
        fprintf(stderr, "\nFile \"%s\" Opening ERROR\n", STUDS_DATA_FILE_NAME);
        return 0;
    }

    //fseek(fp, 0, SEEK_END);
    //int num_of_records_in_f = ftell(fp) / sizeof(struct File_record);
    int num_of_records_in_f = lseek(fd, 0, SEEK_END) / sizeof(struct File_record);

    //fclose(fp);
    close(fd);
    return num_of_records_in_f;
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

    return 1;
}

void choosen_data_info() {
    if (chosen_student_is_chosened == 0) {
        printf("\nYou didn't choose Data for Modifying\n");
    }
    else if (chosen_student_is_modified == 0) {
        printf("\nChosen data:\nStud_%d: Name: \"%s\" | Addres: \"%s\" | Semester: \"%d\"\nWasn't modifyed.\n", chosen_student_index + 1, chosen_student.name, chosen_student.address, chosen_student.semester);
    }
    else {
        printf("\nOld data for modifying:\nStud_%d: Name: \"%s\" | Addres: \"%s\" | Semester: \"%d\"", chosen_student_index + 1, old_chosen_student_info.name, old_chosen_student_info.address, old_chosen_student_info.semester);
        printf("\nModifyed Data:\nStud_%d: Name: \"%s\" | Addres: \"%s\" | Semester: \"%d\"\n", chosen_student_index + 1, chosen_student.name, chosen_student.address, chosen_student.semester);
    }
}



/// ----------- Modify Choosen data   ----------------------------------------------------------------------------------------
void modify_stud_data() {

    if (chosen_student_is_chosened == 0) {
        printf("\nStudent info is not chosen\n");
        return;
    }

    //printf("\nOld data for modifying:\nStud_%d: Name: \"%s\" | Addres: \"%s\" | Semester: \"%d\"", chosen_student_index + 1, old_chosen_student_info.name, old_chosen_student_info.address, old_chosen_student_info.semester);
    printf("\nChosen data for modifying:\nStud_%d: Name: \"%s\" | Addres: \"%s\" | Semester: \"%d\"\n", chosen_student_index + 1, chosen_student.name, chosen_student.address, chosen_student.semester);
    printf("Choose field for modifying:\n"
           "  1 -- Name\n"
           "  2 -- Adress\n"
           "  3 -- Semester\n");
    char choice_string [STUD_NAME_LEN];
    if (str_input(choice_string, STUD_NAME_LEN) == 0) {
        return;
    }
    switch (choice_string[0]) {
    case '1':
        //printf("\nIt is option 1\n");
        printf("\nEnter new student name:\n");
        if (str_input(choice_string, STUD_NAME_LEN) == 0) {
            return;
        }
        strcpy(chosen_student.name, choice_string);
        chosen_student_is_modified = 1;
        printf("\nStudent name is redacted\n");
        break;

    case '2':
        //printf("\nIt is option 2\n");
        printf("\nEnter new student address:\n");
        if (str_input(choice_string, STUD_NAME_LEN) == 0) {
            return;
        }
        strcpy(chosen_student.address, choice_string);
        chosen_student_is_modified = 1;
        printf("\nStudent address is redacted\n");
        break;

    case '3':
        //printf("\nIt is option 3\n");
        printf("\nEnter new student's semester:\n");
        if (str_input(choice_string, STUD_NAME_LEN) == 0) {
            return;
        }
        int semester_ind = atoi(choice_string);
        if (semester_ind == 0) {
            fprintf(stderr, "\nWrong Semester Value\n");
        }
        else {
            chosen_student.semester = semester_ind;
            printf("\nStudent's semester is redacted\n");
            chosen_student_is_modified = 1;
        }
        break;

    default: 
        fprintf(stderr, "\nInvalid option \n");
    }
}


void save_mod_data() {

    if (chosen_student_is_chosened == 0) {
        printf("\nStudent info was not chosen.\n");
        return;
    }
    if (chosen_student_is_modified == 0) {
        printf("\nStudent info was not modified.\n");
        return;
    }

    int fd = open(STUDS_DATA_FILE_NAME, O_RDWR, 0644);
    if (fd == -1) {
        fprintf(stderr, "\nFile Opening ERROR\n");
        exit(1);
    }

    struct flock file_lock;
    file_lock.l_type = F_WRLCK;
    file_lock.l_whence = SEEK_SET;
    file_lock.l_start = chosen_student_index * sizeof(struct File_record);
    file_lock.l_len = sizeof(struct File_record);

    int res = fcntl(fd, F_SETLK, &file_lock);
    if (res == -1) {
        fprintf(stderr, "\nEntered data is locked. Try later please\n");
        close(fd);
        return;
    }

///
    printf("\nLock on entered record is determined. Saving data process...\n");
    sleep(4);

    struct File_record new_stud_rec;
    lseek(fd, chosen_student_index * sizeof(struct File_record), SEEK_SET);
    read(fd, &new_stud_rec, sizeof(struct File_record));

    if (strcmp(new_stud_rec.name, old_chosen_student_info.name) == 0 && strcmp(new_stud_rec.address, old_chosen_student_info.address) == 0 && new_stud_rec.semester == old_chosen_student_info.semester) {
        //printf("\nThe same\n");
        lseek(fd, chosen_student_index * sizeof(struct File_record), SEEK_SET);
        write(fd, &chosen_student, sizeof(struct File_record));
        chosen_student_is_chosened = 0;
        chosen_student_is_modified = 0;
        //sleep(8);
    }
    else {
        printf("\nChosen record has been changed in other program. \nYour modifications have been canceled.\n");
            file_lock.l_type = F_UNLCK;
            res = fcntl(fd, F_SETLK, &file_lock);
            if (res == -1) {
                fprintf(stderr, "\nFcntl Unlock ERROR\n");
                close(fd);
                exit(1);
            }

        old_chosen_student_info = new_stud_rec;
        chosen_student = new_stud_rec;
        chosen_student_is_modified = 0;
        close(fd);
        return;
    }

    //sleep(10);

    file_lock.l_type = F_UNLCK;
    res = fcntl(fd, F_SETLK, &file_lock);
    if (res == -1) {
        fprintf(stderr, "\nFcntl Unlock ERROR\n");
        close(fd);
        return;
    }

    printf("\nNew record info is successfully saved\n");

    close(fd);
}
