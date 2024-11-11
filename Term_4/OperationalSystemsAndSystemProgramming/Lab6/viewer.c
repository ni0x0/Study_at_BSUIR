

#include "header.h"

struct index_s index_record;


/// // MAIN // ///
int main (int argc, char *argv[])
{
    struct index_hdr_s records_info;
    char* file_name;
    //int index_size;

    if (argc == 2) {
        file_name = argv[1];
    }
    else {
        fprintf(stderr, "There are default values. If you want your values the program must be run:\n\"./generator filename\"\n");
        //exit(1);
        file_name = DEFAULT_FILE_NAME;
    }

    int fd = open(file_name, O_RDWR, S_IRUSR | S_IWUSR);
    //printf("{{%d}}", fd);
    if (fd == -1) {
        fprintf(stderr, "File %s Opening ERROR", file_name);
        exit(1);
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        fprintf(stderr, "fstat ERROR\n");
        exit(-1);
    }

    close (fd);
    records_info.recsords = sb.st_size / 16;
    //printf("f size:[%ld]", sb.st_size);

    FILE * f = fopen(file_name, "rb");
    if (f == NULL) {
        fprintf(stderr, "File %s Opening ERROR", file_name);
        exit(1);
    }

    //fread(&(records_info.recsords), sizeof(uint64_t), 1, f);
    //fread(&(records_info.recsords), sizeof(uint64_t), 1, f);

    //struct index_s ind_s [5];
    //fread(ind_s, sizeof(struct index_s), 5, f);
    //for (int i = 0; i < 5; i++) {
    //    printf("[%f]\n", ind_s[i].time_mark);
    //}

    
    struct termios oldattr, newattr;
    tcgetattr(STDIN_FILENO, &oldattr);
    newattr = oldattr;
    newattr.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr);

    char ch;
    uint64_t readed_num = 0;

    struct index_s i_idx;
    //printf("Struct:[%ld]\n", sizeof(struct index_s));
    printf("All:%ld. Read:%ld. Press \'w\' to up, \'s\' to down, \'q\' to quit.\n", records_info.recsords, readed_num);

    while (1) {
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            if (ch == 'q') {
                //printf("\n");
                break;
            }
            //printf("You pressed '%c'\n", ch);
            /// Dowm
            if (ch == 's' && readed_num < records_info.recsords) {
                system("clear");

                if (readed_num < NOFNIN_ROW*NOFNIN_COLUMN ) {
                    //fseek(f, 8, SEEK_SET);
                    //fseek(f, 16, SEEK_SET);
                    fseek(f, 0, SEEK_SET);
                    readed_num = 0;
                }
                else {
                    fseek(f, -(NOFNIN_ROW*(NOFNIN_COLUMN - 1)*16), SEEK_CUR);
                    readed_num -= 5*(NOFNIN_COLUMN - 1);
                }

                for (int i = 1; i <= NOFNIN_ROW*NOFNIN_COLUMN && readed_num < records_info.recsords; i++) {
                    fread(&i_idx, sizeof(struct index_s), 1, f);
                    readed_num++;
                    printf("%-12f  ", i_idx.time_mark);

                    if (i % NOFNIN_ROW == 0) { // && i != 0) {
                        printf("\n");
                    }                   
                    //printf("You pressed '%c'\n", ch);

                }

                printf("\nAll:%ld. Read:%ld. Press \'w\' to up, \'s\' to down, \'q\' to quit.\n", records_info.recsords, readed_num);

            }

            
            /// Up
            if (ch == 'w' && readed_num > NOFNIN_ROW*NOFNIN_COLUMN) {
                system("clear");

                if (readed_num == records_info.recsords && records_info.recsords % NOFNIN_ROW != 0) {
                    uint64_t offset_rn = records_info.recsords - NOFNIN_ROW*(records_info.recsords / NOFNIN_ROW);

                    fseek(f, -(offset_rn*16), SEEK_CUR);
                    readed_num -= offset_rn;
                }

                    fseek(f, -(NOFNIN_ROW*(NOFNIN_COLUMN + 1)*16), SEEK_CUR);
                    readed_num -= NOFNIN_ROW*(NOFNIN_COLUMN + 1);
                

                for (int i = 1; i <= NOFNIN_ROW*NOFNIN_COLUMN && readed_num < records_info.recsords; i++) {
                    fread(&i_idx, sizeof(struct index_s), 1, f);
                    readed_num++;
                    printf("%-12f  ", i_idx.time_mark);

                    if (i % NOFNIN_ROW == 0) { // && i != 0) {
                        printf("\n");
                    }                   
                    //printf("You pressed '%c'\n", ch);

                }

                printf("\nAll:%ld. Read:%ld. Press \'w\' to up, \'s\' to down, \'q\' to quit.\n", records_info.recsords, readed_num);

            }


        }
    }


    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);
    fclose(f);

    exit(0);
}


    //printf("Num:[%ld]\n", records_info.recsords);


    //struct index_s i_idx;
    //for (long unsigned int i = 0; i < records_info.recsords; i++) {
    //    fread(&i_idx, sizeof(struct index_s), 1, f);
    //    printf("%f  ", i_idx.time_mark);

    //    if (i % 6 == 0) {
    //        printf("\n");
    //    }
    //}

    //for (int i = 1; i < 30; i++) {
    //    printf("[%d]\n", i);
    //}

    /*
    struct index_s i_idxes [24];


    struct index_s i_idx2;
    struct index_s i_idx3;

    fread(&i_idx, sizeof(struct index_s), 1, f);
    fread(&i_idx2, sizeof(struct index_s), 1, f);
    fread(&i_idx3, sizeof(struct index_s), 1, f);


    printf("struct index_s:[%ld]  uint64_tsize:[%ld]  f_pos:[%ld]\n", sizeof(struct index_s), sizeof(uint64_t), ftell(f));
    printf("[%f]  [%f]  [%f]", i_idx.time_mark, i_idx2.time_mark, i_idx3.time_mark);
    fseek(f, -32, SEEK_CUR);
    fread(&i_idx3, sizeof(struct index_s), 1, f);
    printf("[%f]\n", i_idx3.time_mark);

    printf("\n"); 
    */ 






    /*





    //records_info.recsords = index_size;
    records_info.idx = (struct index_s *)calloc(records_info.recsords, sizeof(struct index_s));
    if (records_info.idx == NULL) {
        fprintf(stderr, "Calloc ERROR\n");
        fclose(f);
        exit(1);
    }

    fread(records_info.idx, sizeof(struct index_s), records_info.recsords, f);
    

    printf("\nFirst one: [%f]\nSecond one:[%f]\n", records_info.idx[0].time_mark, records_info.idx[records_info.recsords-1].time_mark);

    for (long unsigned int i = 0; i < records_info.recsords; i++) {
        printf("%f  ", records_info.idx[i].time_mark);
    }


    free(records_info.idx);

    */
