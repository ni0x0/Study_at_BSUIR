#include "database_opts.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////    GET / DEL  DIRS //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
///   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/// Add to db
void add_directory_for_monit() {
    char dir_name [FILE_NAME_LEN];

    printf("Enter Directory Name and press Enter:\n");

    // Get Dir Name
    if (str_input(dir_name, FILE_NAME_LEN) == 0){
        return;
    }
    //printf("Entered String:[%s]\n", dir_name);


    // Check on Dir Type
    struct stat sb;
    if (lstat(dir_name, &sb) == -1) {     
        fprintf(stdout, "Entered file is not found.\n"); 
        return;
    }
    if ((sb.st_mode & S_IFMT) != S_IFDIR) {
        printf("Entered file is not a directory.\n"); 
        return;
    }
    //printf("Good Directory)\n"); 
   
    // Get full path
    if (get_full_dir_path(dir_name) == 0) {
        fprintf(stderr, "Realpath Getting ERROR\n");
        return;
    }

    // Check dir on Presense
    if (check_key_presence_in_db_tree (DIRS_DATABASE_NAME, dir_name, 0) != 0) {
        printf("Entered Directory is already monitored.\n"); 
        return;
    }

    // Check dir on Subdirectory
    unsigned check_d = check_dirs_on_sub(dir_name);
    if (check_d == 1) {
        printf("Entered Directory is a subdirectory of already monitored one.\n"); 
        return;
    } else if (check_d == 2) {
        printf("Entered Directory has a subdirectory among of already monitored ones.\n"); 
        return;
    }


    // Add files to database
    //dir_run_save_files_in_db (dir_name, FILES_DATABASE_NAME);          

    // Add Dir to Database
    char size_str[VALUE_STR_SIZE];
    sprintf(size_str, "%ld", sb.st_size);
    add_to_tree_database(DIRS_DATABASE_NAME, size_str, dir_name, 0); 
    

    printf("\nDirectory:[%s] successfully added for monitoring\n", dir_name);
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

/// Get Full Dir Path
unsigned get_full_dir_path(char* dir_name) {

    if (dir_name[0] != '/') {
        char relative_name [FILE_NAME_LEN];
        strcpy(relative_name, dir_name);
        char* resolved_path = realpath(relative_name, dir_name);
            if (resolved_path == NULL) {
                //fprintf(stderr, "Realpath Getting ERROR\n");
                return 0;
            }
    }
    if (dir_name[strlen(dir_name) - 1] != '/' ) 
        strcat(dir_name, "/");

    return 1;
}


/// Check on presense in DB by key
unsigned check_key_presence_in_db_tree (char* db_name, char* db_key, unsigned dups_fl) {
    DB *dbp;
    int ret;
    DBT kkey, ddata;

    dbp = open_tree_database(db_name, dups_fl);
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    kkey.data = db_key;
    kkey.size = strlen(db_key) + 1;

    ret = dbp->get(dbp, NULL, &kkey, &ddata, 0);
    //if (ret != 0) {
    //    fprintf(stderr, "(%d)DB_GET ERROR: %s\n", ret, db_strerror(ret));
    //    //exit(EXIT_FAILURE);
    //}else 
    //    printf("===%s: %s\n", (char *) kkey.data, (char *) ddata.data);

    dbp->close(dbp, 0);
    return (!ret);
}

/// Process Chosen DIRs on Subdirs 
// 0 - not subdir, 1 - is subdir, 2 - has subdir  
unsigned check_dirs_on_sub(char* dir_name) {
    
    DB * dbp = open_tree_database(DIRS_DATABASE_NAME, 0);
    if (get_num_of_records(dbp) == 0) {
        dbp->close(dbp, 0);
        return 0;
    }
 
    // Get  Values
    DBT kkey, ddata;
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    DBC *cursor;
    int ret = dbp->cursor(dbp, NULL, &cursor, 0); 
    if (ret != 0) {
        fprintf(stderr, "CURSOR CREATE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }       
   
    // Proccess 
    ret = cursor->c_get(cursor, &kkey, &ddata, DB_FIRST);
    while (ret == 0) {
        //printf("Curr DIR {%s}\n", ((char *)kkey.data));

        if (strncmp(dir_name, (char *)kkey.data, strlen((char *)kkey.data)) == 0) {
            return 1;
        } else if (strncmp(dir_name, (char *)kkey.data, strlen(dir_name)) == 0) {
            return 2;
        }

        ret = cursor->c_get(cursor, &kkey, &ddata, DB_NEXT);
    }

    ret = cursor->c_close(cursor);
    if (ret != 0) {
        fprintf(stderr, "CURSOR CLOSE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }
    dbp->close(dbp, 0);

    return 0;
}

/// Add to tree Database
void add_to_tree_database(char* db_name, char* db_data, char* db_key, unsigned dups_fl) {
    //printf("\n1:[%s]\n2:[%s]\n3:[%s]\n", db_name, db_data, db_key);
    DB *dbp;
    int ret;
    DBT kkey, ddata;

    dbp = open_tree_database(db_name, dups_fl);

    // Add values
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    kkey.data = db_key;
    kkey.size = strlen(db_key) + 1;

    ddata.data = db_data;
    ddata.size = strlen(db_data) + 1;

    ret = dbp->put(dbp, NULL, &kkey, &ddata, 0);
    if (ret != 0) {
        fprintf(stderr, "DATA PUT ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    dbp->close(dbp, 0);
}

/// Open DB 
DB * open_tree_database(char* db_name, unsigned dups_fl) {
    DB *dbp;
    int ret;

    // Create DataBase
    ret = db_create(&dbp, NULL, 0);
    if (ret != 0) {
        fprintf(stderr, "%s db_create ERROR: %s\n", db_name, db_strerror(ret));
        exit(1);
    }

    if (dups_fl) {
        ret = dbp->set_flags(dbp, DB_DUP);
        if (ret != 0) {
            fprintf(stderr, "FLAGS ERROR: %s\n", db_strerror(ret));
            exit(1);
        }
    }

    // Open DataBase
    ret = dbp->open(dbp, NULL, db_name, NULL, DB_BTREE, DB_CREATE, 0664);
    if (ret != 0) {
        fprintf(stderr, "%s db_open ERROR: %s\n", db_name, db_strerror(ret));
        exit(1);
    }

    return dbp;
}




/// Get List of Dirs
void show_list_of_monitored_directories() {

    printf("\nList of Monitored Directories:\n");
    print_db_tree_content(DIRS_DATABASE_NAME, 0, 0);

}

/// Print DB Content
void print_db_tree_content(char* db_name, unsigned print_vals_fl, unsigned dups_fl) {

    DB *dbp;
    int ret;
    DBT kkey, ddata;

    dbp = open_tree_database(db_name, dups_fl);
   
    // Print out the number of records in the database
    printf("[%s] contains %lu records\n", db_name, get_num_of_records(dbp));

    // Get  Values
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    DBC *cursor;
    ret = dbp->cursor(dbp, NULL, &cursor, 0); 
    if (ret != 0) {
        fprintf(stderr, "CURSOR CREATE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    ret = cursor->c_get(cursor, &kkey, &ddata, DB_FIRST);
    int i = 1;
    while (ret == 0) {
        //printf(" database contains 1: %s - %s\n", (char *)kkey.data, (char *)ddata.data);
        printf("%d. %s", i++, (char *)kkey.data);
        if (print_vals_fl) {
            printf(" -- %s\n", (char *)ddata.data);
        }
        else 
            printf("\n");
        ret = cursor->c_get(cursor, &kkey, &ddata, DB_NEXT);
    }
    if (i == 1) {
        printf("There is no data\n");
    }

    ret = cursor->c_close(cursor);
    if (ret != 0) {
        fprintf(stderr, "CURSOR CLOSE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    dbp->close(dbp, 0);
}

/// Get Num of Elems
u_long get_num_of_records(DB *dbp) {
    int ret;
    DB_BTREE_STAT *statp;
    if ((ret = dbp->stat(dbp, NULL, &statp, 0)) != 0) {
            dbp->err(dbp, ret, "DB->stat");
            return -1;
    }
    u_long num = (u_long)statp->bt_ndata;
    
    free(statp);

    return num;
}





/// Del Dir
void delete_directory_for_monit() {
    show_list_of_monitored_directories();

    // Get Num of Dirs
    DB * dbp = open_tree_database(DIRS_DATABASE_NAME, 0);
    u_long dirs_num = get_num_of_records(dbp);
    dbp->close(dbp, 0);
    if (dirs_num == 0) {
        //printf("No directories to delete\n");
        return;
    }

    // Get Dir Num to Del
    printf("Choose Num of the Directory to delete and press Enter:\n");
    char num_str[VALUE_STR_SIZE];
    if (str_input(num_str, VALUE_STR_SIZE) == 0){
        return;
    }
    u_long ch_num = num_str_check(num_str, 1, dirs_num);
    if (ch_num == 0) {
        printf("Entered wrong value\n");
        return;
    }
    //printf("Entered Directory Num:[%ld]\n", ch_num);


    // Del dir
    dbp = open_tree_database(DIRS_DATABASE_NAME, 0);

    DBC * cursor = get_cursor_by_num(dbp, ch_num);
    
    DBT kkey, ddata;
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));
    int ret = cursor->c_get(cursor, &kkey, &ddata, DB_CURRENT);
    printf("\nDirectory %s is successfully deleted from Monitoring List\n", (char *)kkey.data);

    //del_dir_files_from_db ((char *)kkey.data);   !!!!!!!!!!!!!!!!

    cursor->c_del(cursor, 0);
    ret =cursor->c_close(cursor);
    if (ret != 0) {
        fprintf(stderr, "CURSOR CLOSE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }
    dbp->close(dbp, 0);
}

/// Check str on Num
int num_str_check(char* str, int num1, int num2) {
    int num_to_ret;
    for (int i = 0; str[i]; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    num_to_ret = atoi(str);
    if (num_to_ret < num1 || num_to_ret > num2) {
        return 0;
    }
    return num_to_ret;
}

/// Get cursor by Num
DBC * get_cursor_by_num(DB *dbp, u_long num) {

    DBC *cursor;
    DBT kkey, ddata;
    int ret;
    ret = dbp->cursor(dbp, NULL, &cursor, 0); 
    if (ret != 0) {
        fprintf(stderr, "CURSOR CREATE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));
    ret = cursor->c_get(cursor, &kkey, &ddata, DB_FIRST);
    int i = 1;
    while (ret == 0 && (u_long)i != num) {
        //printf("%d. %s\n", i, (char *)ddata.data);
        ret = cursor->c_get(cursor, &kkey, &ddata, DB_NEXT);
        i++;
    }
    //printf("MIIIINNEE%ld. %s\n", num, (char *)ddata.data);
    return cursor;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////    START   DAEMON     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
/// 
/// 
/// 
//int fd;    // inotify


/// Clear Database
void clear_database(char* db_name, unsigned dups_fl) {

    DB * dbp = open_tree_database(db_name, dups_fl);
    if (dbp->truncate(dbp, NULL, NULL, 0) != 0) {
        fprintf (stderr, "CLEAR DATABASE %s ERROR\n", db_name);
        exit(1);
    }
    dbp->close(dbp, 0);
}


/// Process Chosen DIRs
unsigned chosen_dirs_process(char* log_f_name) {
    
    DB * dbp = open_tree_database(DIRS_DATABASE_NAME, 0);
    if (get_num_of_records(dbp) == 0) {
        dbp->close(dbp, 0);
        return 0;
    }

    // Inotify init
    int fd = inotify_init();
    if (fd < 0) {
        fprintf(stderr, "inotify_init ERROR\n");
        exit(1);
    }
    save_int_data(FD_INOT_FILE_NAME, fd);

    // Get  Values
    DBT kkey, ddata;
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    DBC *cursor;
    int ret = dbp->cursor(dbp, NULL, &cursor, 0); 
    if (ret != 0) {
        fprintf(stderr, "CURSOR CREATE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    char dirrr [FILE_NAME_LEN];    
    // Proccess 
    ret = cursor->c_get(cursor, &kkey, &ddata, DB_FIRST);
    while (ret == 0) {
        //printf("Curr DIR {%s}\n", ((char *)kkey.data));
        strcpy(dirrr, (char *)kkey.data);
        //printf("New key {%s}\n", dirrr);

        dir_run_save_files_in_db (dirrr, FILES_DATABASE_NAME, log_f_name);
        ret = cursor->c_get(cursor, &kkey, &ddata, DB_NEXT);
    }


    ret = cursor->c_close(cursor);
    if (ret != 0) {
        fprintf(stderr, "CURSOR CLOSE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }
    dbp->close(dbp, 0);
    return 1;
}


void save_int_data(char* f_name, int _int_n) {
    FILE * data_file = fopen(f_name, "w");
    if (!data_file) {
        fprintf(stderr, "Creating %s file ERROR.\n", f_name);
        exit(1);
    }

    fprintf(data_file, "%d", _int_n);
    fclose(data_file);
}


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


// 0 - needed to add in base
unsigned file_processing(struct stat * s, char* full_f_name, char* log_f_name) {
    if (!(S_ISREG(s->st_mode))){
        //printf("{%s} is not REG\n", full_f_name);
        return 1;
    }

    char size_str[VALUE_STR_SIZE];
    sprintf(size_str, "%ld", s->st_size);
    if (check_key_presence_in_db_tree(FILES_DATABASE_NAME, size_str, 1) == 0) {
        return 0;
    }
    //printf("{%s} is already presense\n", full_f_name);

    // Base has same size files. Need to check hash

    DB * dbp = open_tree_database(FILES_DATABASE_NAME, 1);
    DBT kkey, ddata;
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    kkey.data = size_str;
    kkey.size = strlen(size_str) + 1;

    DBC *cursor;
    dbp->cursor(dbp, NULL, &cursor, 0); 

    int ret = cursor->get(cursor, &kkey, &ddata, DB_SET);   

    unsigned same = 0;
    while (ret == 0 && strcmp(size_str, (char *)kkey.data) == 0) {
        //printf("=====: %s - %s\n", (char *)kkey.data, (char *)ddata.data);

        // hash check
        //if (check_file_presense((char *)ddata.data) == 1) {
        
        if (files_hash_compare(full_f_name, (char *)ddata.data) != 0) {
            //printf("{%s} has same hash as {%s}\n", full_f_name, (char *) ddata.data);
            same = 1;

            // Replace on lync
            link_create ((char *) ddata.data, full_f_name, log_f_name);
            break;
        }

        //}

        ret = cursor->get(cursor, &kkey, &ddata, DB_NEXT);
    }

    if (same == 0) {
        //printf("{%s} is different\n", full_f_name);
        //add_data_in_file(log_f_name, "\n-----\n");
        //add_data_in_file(log_f_name, full_f_name);
        //add_data_in_file(log_f_name, " is diff\n");
        return 0;
    }

    return 1;   
}


////////////////                          /////////////////////
////////////////  INOT & DAEMON FUNCTIONS ///////////////////// 
////////////////                          /////////////////////




void inot_close() {
    //add_data_in_file("/home/niks0/MNFolder/oslabs/kurs_w_dirs/build/loq.txt", "\nInot close startw\n");
    DB * dbp = open_tree_database(INOT_SUBDIRS_DATABASE_NAME, 0);
    //add_data_in_file("/home/niks0/MNFolder/oslabs/kurs_w_dirs/build/loq.txt", "\nInot close after tree open\n");

    // Get  Values
    DBT kkey, ddata;
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    DBC *cursor;
    int ret = dbp->cursor(dbp, NULL, &cursor, 0); 
    if (ret != 0) {
        fprintf(stderr, "CURSOR CREATE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }
    int fd = get_int_data(FD_INOT_FILE_NAME);

    // Proccess 
    ret = cursor->c_get(cursor, &kkey, &ddata, DB_FIRST);
    while (ret == 0) {
        //printf("WD:[%d], {%s}\n", atoi(((char *)kkey.data)), ((char *)ddata.data));
        //(void) inotify_rm_watch(fd, wd);

        //add_data_in_file("/home/niks0/MNFolder/oslabs/kurs_w_dirs/build/loq.txt", "\nInot RM ---+---\n");
        int wddel = atoi(((char *)kkey.data));
        (void) inotify_rm_watch(fd, wddel);
        //add_data_in_file("/home/niks0/MNFolder/oslabs/kurs_w_dirs/build/loq.txt", "\nInot RM after ---+---\n");

        ret = cursor->c_get(cursor, &kkey, &ddata, DB_NEXT);
    }


    ret = cursor->c_close(cursor);
    if (ret != 0) {
        fprintf(stderr, "CURSOR CLOSE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }
    dbp->close(dbp, 0);


    (void) close(fd);
}

int get_int_data(char* f_name) {
    FILE * _file = fopen(f_name, "r");
    if (!_file) {
        return 0;
    }

    int _int_n;
    fscanf(_file, "%d", &_int_n);
    fclose(_file);

    return _int_n;
}




/// Get Data By Key
unsigned get_data_by_key_in_db_tree (char* db_name, char* db_key, char* db_data, unsigned dups_fl) {
    DB *dbp;
    int ret;
    DBT kkey, ddata;

    dbp = open_tree_database(db_name, dups_fl);
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    kkey.data = db_key;
    kkey.size = strlen(db_key) + 1;

    ret = dbp->get(dbp, NULL, &kkey, &ddata, 0);
    if (ret == 0) {
        strcpy(db_data, (char *) ddata.data);
    }

    dbp->close(dbp, 0);
    return (!ret);
}













void inot() {
    int length, i = 0;
    int fd;
    int wd, wd2;
    char buffer[BUF_LEN];

    fd = inotify_init();

    if (fd < 0) {
        perror("inotify_init");
    }

    wd = inotify_add_watch(fd, "/home/niks0/MNFolder/oslabs/kurs_w_dirs/test/dups/", IN_CLOSE_WRITE | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MODIFY | IN_MOVED_TO | IN_MOVED_FROM | IN_MOVE_SELF);
    wd2 = inotify_add_watch(fd, "/home/niks0/MNFolder/oslabs/kurs_w_dirs/test/", IN_CLOSE_WRITE | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MODIFY | IN_MOVED_TO | IN_MOVED_FROM | IN_MOVE_SELF);

    while (1) {
        i = 0;
        // считываем данные в буфер
        length = read(fd, buffer, BUF_LEN);

        // проверяем успешность чтения
        if (length < 0) {
            perror("read");
        }

        // обрабатываем события в буфере
        while (i < length) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->wd == wd) {
                printf("\n In /test/dups/\n");
            }
            else 
                printf("\n In /test/\n");

            if (event->mask & IN_ISDIR) {
                printf("The directory %s ", event->name);
            }
            else 
                printf("The file %s ", event->name);


            if (event->mask & IN_CLOSE_WRITE) {
                printf("was write close.\n");
            }
            if (event->mask & IN_CREATE) {
                printf("was created.\n");
            }
            if (event->mask & IN_DELETE) {
                printf("was deleted.\n");
            }
            if (event->mask & IN_DELETE_SELF) {
                printf("was deleted selff.\n");
            }

            if (event->mask & IN_MODIFY) {
                printf("was modifyd.\n");
            }
            if (event->mask & IN_MOVED_TO) {
                printf("was moved to.\n");
            }
            if (event->mask & IN_MOVED_FROM) {
                printf("was moved from.\n");
            }
            if (event->mask & IN_MOVE_SELF) {
                printf("was moved self.\n");
            }


            // переходим к следующему событию в буфере
            i += EVENT_SIZE + event->len;
        }
    }

    (void) inotify_rm_watch(fd, wd);
    (void) inotify_rm_watch(fd, wd2);
    (void) close(fd);

    exit(EXIT_SUCCESS);

}




void del_dir_files_from_db (char * dir_name) {

    DB *dbp;
    int ret;
    DBT kkey, ddata;

    dbp = open_tree_database(FILES_DATABASE_NAME, 1);

    // Initialize values
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    DBC *cursor;
    ret = dbp->cursor(dbp, NULL, &cursor, 0); 
    if (ret != 0) {
        fprintf(stderr, "CURSOR CREATE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    ret = cursor->c_get(cursor, &kkey, &ddata, DB_FIRST);
    int i = 1;
    while (ret == 0) {
        if (strncmp((char *)ddata.data, dir_name, strlen(dir_name)) == 0) {
            //printf("%d. %s -- %s\n", i, (char *)kkey.data, (char *)ddata.data);
            cursor->del(cursor, 0); 
        }
        
        i++;
        ret = cursor->c_get(cursor, &kkey, &ddata, DB_NEXT);
    }

    ret =cursor->c_close(cursor);
    if (ret != 0) {
        fprintf(stderr, "CURSOR CLOSE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    dbp->close(dbp, 0);
}



void del_data_db (char * db_name, char* data_name) {

    DB *dbp;
    int ret;
    DBT kkey, ddata;

    dbp = open_tree_database(db_name, 1);

    // Initialize values
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    DBC *cursor;
    ret = dbp->cursor(dbp, NULL, &cursor, 0); 
    if (ret != 0) {
        fprintf(stderr, "CURSOR CREATE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    ret = cursor->c_get(cursor, &kkey, &ddata, DB_FIRST);

    while (ret == 0) {
        if (strcmp((char *)ddata.data, data_name) == 0) {
            //printf("%d. %s -- %s\n", i, (char *)kkey.data, (char *)ddata.data);
            cursor->del(cursor, 0); 
            //break;
        }
        
        ret = cursor->c_get(cursor, &kkey, &ddata, DB_NEXT);
    }

    ret =cursor->c_close(cursor);
    if (ret != 0) {
        fprintf(stderr, "CURSOR CLOSE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    dbp->close(dbp, 0);
}


void del_dirs_wds (char * dir_name) {

    DB *dbp;
    int ret;
    DBT kkey, ddata;

    dbp = open_tree_database(INOT_SUBDIRS_DATABASE_NAME, 0);

    // Initialize values
    memset(&kkey, 0, sizeof(kkey));
    memset(&ddata, 0, sizeof(ddata));

    DBC *cursor;
    ret = dbp->cursor(dbp, NULL, &cursor, 0); 
    if (ret != 0) {
        fprintf(stderr, "CURSOR CREATE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }
    int fd = get_int_data(FD_INOT_FILE_NAME);
    ret = cursor->c_get(cursor, &kkey, &ddata, DB_FIRST);
    while (ret == 0) {
        if (strncmp((char *)ddata.data, dir_name, strlen(dir_name)) == 0) {
            int wdrm = atoi(((char *)kkey.data));
            (void) inotify_rm_watch(fd, wdrm);

            cursor->del(cursor, 0); 
        }
        
        ret = cursor->c_get(cursor, &kkey, &ddata, DB_NEXT);
    }

    ret =cursor->c_close(cursor);
    if (ret != 0) {
        fprintf(stderr, "CURSOR CLOSE ERROR: %s\n", db_strerror(ret));
        exit(1);
    }

    dbp->close(dbp, 0);
}

