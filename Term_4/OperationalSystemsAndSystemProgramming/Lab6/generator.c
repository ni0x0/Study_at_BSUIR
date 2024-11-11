#include "header.h"


/// // MAIN // ///
int main (int argc, char *argv[])
{
    char* file_name;
    int index_size;

    if (argc == 3) {
        file_name = argv[1];
        index_size = atoi(argv[2]);
    }
    else {
        fprintf(stderr, "There are default values. If you want your values the program must be run:\n\"./generator filename index_size\"\n");
        //exit(1);
        file_name = DEFAULT_FILE_NAME;
        index_size = INDEX_DEFAULT_SIZE;

    }
    if (index_size == 0 || index_size % INDEX_DEFAULT_MIN_SIZE != 0) {
        fprintf(stderr, "Index value must be multiple of %d\n", INDEX_DEFAULT_MIN_SIZE);
        exit(1);
    }

    //printf("Final Data:\nFilename: [%s]\nIndex size: [%d]\n", file_name, index_size); 
    //printf("Page size:[%d]\n", getpagesize());

    FILE * f = fopen(file_name, "wb");
    if (f == NULL) {
        fprintf(stderr, "File Open ERROR");
        exit(1);
    }

    /// Generation process
    printf("\nStarting Generation process...\n"); 



    srand(time(NULL));
    double int_part; 
    double fract_part; 
    struct index_s i_idx;
    /// Generation cycle 

    for (int i = 0; i < index_size; i++) {

        //long int_part = (rand() % (time(0) - ONE_DAY_SECONDS - START_TIME)) + START_TIME;
        int_part = (double)(rand() % (CURR_MODIF_JD - START_TIME)) + START_TIME;
        fract_part = (double)(rand()) / RAND_MAX * FRACT_MARK;

        i_idx.time_mark = int_part + fract_part;
        i_idx.recno = i;
        fwrite(&i_idx, sizeof(struct index_s), 1, f);

        //printf("%f  ", i_idx.time_mark);
    } 


    printf("\n%d values are successfully generated and saved in \"%s\"\n", index_size, file_name); 
    fclose(f);

    exit(0);
}









/*
    fwrite(&(records_info.recsords), sizeof(uint64_t), 1, f);
    fwrite(records_info.idx, sizeof(struct index_s), index_size, f);

    //printf("\nSize:[%ld]\n", sizeof(records_info));

    printf("\nFirst one: [%f]\nSecond one:[%f]\n", records_info.idx[0].time_mark, records_info.idx[index_size-1].time_mark);
    //printf("Now time [%ld]\n", time(0));

    free(records_info.idx);

    
    
*/




    //struct index_hdr_s records_info;
    //records_info.recsords = index_size;

    /// 
    //fwrite(&(records_info.recsords), sizeof(uint64_t), 1, f);
    //fwrite(&(records_info.recsords), sizeof(uint64_t), 1, f);
    //double id_size = (double)(records_info.recsords);
    //fwrite(&id_size, sizeof(double), 1, f);
