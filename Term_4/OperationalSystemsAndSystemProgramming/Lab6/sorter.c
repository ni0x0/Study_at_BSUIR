#include "header.h"

struct Data_for_pthread {
    //struct index_s * buff_ptr;
    //unsigned long int buff_size;
    int ind;
};


///Funs 
void input_process(); 
void threads_communication_init();
int compare_fun (const void * a, const void * b);
//qsort(ind_s, 5, sizeof(struct index_s), compare_fun);
int is_power_of(int num, int pow_num);

void * thread_routine(void * arg);
void merge_blocks (struct index_s * ptr, long int num);
int count_nums_in_arr(int * arr, int count_num, int arr_size);
void copy_create(struct stat * sb);

/// Values
struct index_s * ptr;

long int memsize;
int granul;
int threads;
char* file_name;

pthread_t * ptr_threads;
int * blocks_map;
int blocks_mapped_num;
struct Data_for_pthread * i_pthread_data; 
long int blocksize;
int cycles_num;

int merge_barriers_num;

pthread_mutex_t mutte;
pthread_barrier_t m_barrier;


/// // MAIN // /// //// //// ////
int main (int argc, char *argv[])
{
    if (argc == 5) {
        memsize = atoi(argv[1]);
        granul = atoi(argv[2]);
        threads = atoi(argv[3]);
        file_name = argv[4];
    }
    else {
        fprintf(stderr, "There are default values. If you want your values the program must be run:\n\"./sorter memsize granul threads filename\"\n");
        //exit(1);
        memsize = MEMSIZE_DEFAULT_NUM;
        granul = GRANULS_DEFAULT_NUM;
        threads = THREADS_DEFAULT_NUM;
        file_name = DEFAULT_FILE_NAME;
    }

    printf("Final Data:\nMemsize: %ld\nGranul: %d\nThreads: %d\nFilename: \"%s\"\n",  memsize, granul, threads, file_name); 
    input_process();
    threads_communication_init();

/// File Proccessing
    int fd = open(file_name, O_RDWR, S_IRUSR | S_IWUSR);
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        fprintf(stderr, "FILE OPENING ERROR\n");
        exit(-1);
    }

    blocksize = memsize / granul;
    merge_barriers_num = is_power_of(granul, 2);
    printf("File size: %ld\nBlock size: %ld\n", sb.st_size, blocksize);
    //printf("Merge Barrs NUm: [%d]\n", merge_barriers_num);
    int i = 0;
    

    /// 1) threads create 
    int ret;
    
    // Create threads
    for (int i = 0; i < threads; i++) {
        //ptr_threads[i] = i;
        i_pthread_data[i].ind = i;
        ret = pthread_create(&(ptr_threads[i]), NULL, thread_routine, &(i_pthread_data[i]));
        if (ret) {
            fprintf(stderr, "\nCreating Thread ERROR");
            exit(1);
        }
    }


/// 2) Sorting
    //printf("Final Data:\nMemsize: [%ld]\nGranul: [%d]\nThreads: [%d]\nFilename: [%s]\n",  memsize, granul, threads, file_name); 
    cycles_num = sb.st_size / memsize;

//for (i = 0; i < cycles_num; i++) {
            
        memsize = sb.st_size;
        blocksize = memsize / granul;
        //blocksize = sb.st_size;
        
            ptr = mmap(NULL, memsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, i*memsize);//4096);
            copy_create(&sb);

            blocks_mapped_num = threads;
            for (int i = 0; i < granul; i++) {
                blocks_map[i] = 0;
            }
            /*
            printf("\nKart: {");
                for (int i = 0; i < granul; i++) {
                    printf(" %d ", blocks_map[i]); 
                } printf("}\n");
                */
            //sleep(1);
            pthread_barrier_wait(&m_barrier); 
            //printf("\nMain after f barrier (Map Init)\n");
            /// end of ~2)

            pthread_barrier_wait(&m_barrier);
            //printf("\nMain after sec barrier (sort)\n");
            /// end of ~3) ~ sort


            //printf("\n----------------------------------------------------------------\n");
            for (int i = 0; i < merge_barriers_num; i++) {
                pthread_barrier_wait(&m_barrier);
                //printf("\nMain after {%d} merge barrier\n", i + 1);
            }

        munmap(ptr, memsize); 
        //printf("\nSorting is over\n");

//}

    // Waiting for all
    for (int i = 0; i < threads; ++i) {
        pthread_join(ptr_threads[i], NULL);
    }

    /*
        printf("\nKart: {");  
        for (int i = 0; i < granul; i++) {
            printf(" %d ", blocks_map[i]); 
        } printf("}\n");
        */

    printf("\nSorting is over\n");

    pthread_mutex_destroy(&mutte);
    pthread_barrier_destroy(&m_barrier);
    free(blocks_map);
    free(ptr_threads);
    close(fd);

    exit(0);
}






/// CHECK INPUT DATA
void input_process() {
    if (memsize == 0 || memsize % getpagesize() != 0) {
        fprintf(stderr, "Memsize value must be multiple of %d\n", getpagesize());
        exit(1);
    }
    if (threads == 0) {
        fprintf(stderr, "Wrong threads value\n");
        exit(1);
    }
    if (granul == 0 || granul < threads) {
        fprintf(stderr, "Granul value must be greater than num of threads\n");
        exit(1);
    } else if (is_power_of(granul, 2) == 0){
        fprintf(stderr, "Granul value must be power of two\n");
        exit(1);
    }
}


/// DATA FOR THREADS COMMUNICATION ALLOCATION
void threads_communication_init() {
    /// Mem Allocation for threads
    ptr_threads = (pthread_t *)calloc(threads, sizeof(pthread_t));
    if (ptr_threads == NULL) {
        fprintf(stderr, "Pthreads Mem Allocation ERROR\n");
        exit(1);
    }
    blocks_map = (int *)calloc(granul, sizeof(int));
    if (blocks_map == NULL) {
        fprintf(stderr, "Blocks Map Allocation ERROR\n");
        exit(1);
    }
    i_pthread_data = (struct Data_for_pthread *)calloc(threads, sizeof(struct Data_for_pthread));
    if (i_pthread_data == NULL) {
        fprintf(stderr, "Mem for Data for Threads Allocation ERROR\n");
        exit(1);
    }

    int res = pthread_mutex_init(&mutte, NULL);
    if (res) {
        fprintf(stderr, "Mutex Init ERROR\n");
        exit(1);
    }
    res = pthread_barrier_init(&m_barrier, NULL, threads + 1);
    if (res) {
        fprintf(stderr, "Barrier Init ERROR\n");
        exit(1);
    }


}


/// COMPARE NUMS
int compare_fun (const void * a, const void * b) {

    if ((*((struct index_s *)a)).time_mark > (*((struct index_s *)b)).time_mark) {
        return 1;
    }
    else if ((*((struct index_s *)a)).time_mark < (*((struct index_s *)b)).time_mark) {
        return -1;
    }
    else return 0;

}

/// CHECK ON POWER
int is_power_of(int num, int pow_num) {

    if (num == 1) {
        return (pow_num == 1);
    }
    long int pow = 1;
    int i = 0;
    while (pow < num) {
        pow *= pow_num;
        i++;
    }

    if (pow == num) {
        return i;
    }
    else {
        return 0;
    }

}

void copy_create(struct stat * sb) {

    char f_name [4096];
    strcpy(f_name, file_name);
    strcat(f_name, "__unsorted_v"); 
    FILE * copy_file = fopen(f_name, "wb");
    fwrite(ptr, sizeof(struct index_s), sb->st_size /  sizeof(struct index_s), copy_file);
    //fread(&(records_info.recsords), sizeof(uint64_t), 1, f);
    fclose(copy_file);

    printf("\nCopy of your file \"%s\" is created\n", f_name);
}



/// THREAD_ROUTINE
void * thread_routine(void * arg) {

    struct Data_for_pthread this_data = *((struct Data_for_pthread *)arg);
    

//for (int i = 0; i < cycles_num; i++) {

        pthread_barrier_wait(&m_barrier);
        printf("\nThread [%d] is started working\n", this_data.ind);
        //sleep(1);

        /// 3)
    blocks_map[this_data.ind] = 1;


    int nums_num = blocksize / sizeof(struct index_s);
    //if (this_data.ind == 2) {
        qsort(ptr + nums_num * this_data.ind, nums_num, sizeof(struct index_s), compare_fun);
        //printf("\nThread [%d] sorted\n", this_data.ind);
    //}
    //else printf("\n I [%d] don't sort\n", this_data.ind);

        while (1) {
            int taked_block;
                pthread_mutex_lock(&mutte);
                    if (blocks_mapped_num < granul) {
                        taked_block = blocks_mapped_num;
                        blocks_map[taked_block] = 1;
                        blocks_mapped_num++;
                        //printf("\nThread [%d] staked {%d} block\n", this_data.ind, taked_block);
                        pthread_mutex_unlock(&mutte);
                        qsort(ptr + nums_num * taked_block, nums_num, sizeof(struct index_s), compare_fun);
                        //printf("\nThread [%d] sorted {%d} block\n", this_data.ind, taked_block);
                        //sleep(1);
                    }
                    else {
                        //printf("\nThread [%d] waites\n", this_data.ind);
                        pthread_mutex_unlock(&mutte);
                        break;
                    }
                //pthread_mutex_unlock(&mutte);
        }
        
    pthread_barrier_wait(&m_barrier);  /// end of sort (3)


    /// Mergening 
    //int num_of_mergerers = granul;
    int num_of_pairs_for_merge = granul;
    int num_of_nums_in_part_of_pair = nums_num;
         for (int i = 1; i <= merge_barriers_num; i++) {
             num_of_pairs_for_merge /= 2;
             //num_of_nums_in_part_of_pair *= 2; 
             while (1) {
                 int taked_pair;
                    pthread_mutex_lock(&mutte);
                    int merged_pairs_num = count_nums_in_arr(blocks_map, i+1, num_of_pairs_for_merge);
                    if (merged_pairs_num <  num_of_pairs_for_merge) {
                        //taked_block = blocks_mapped_num;
                        blocks_map[merged_pairs_num] = i+1;
                        //blocks_mapped_num++;
                        //printf("\nThread [%d] taked {%d} pair\n", this_data.ind, merged_pairs_num);
                        //printf("\n!!%d!! Thread [%d] taked {%d} pair || num_of_pairs_for_merge: [%d], num_of_nums_in_part_of_pair: [%d] \n", i+1, this_data.ind, merged_pairs_num, num_of_pairs_for_merge, num_of_nums_in_part_of_pair);
                        pthread_mutex_unlock(&mutte);
                        merge_blocks (ptr + num_of_nums_in_part_of_pair*2*merged_pairs_num, num_of_nums_in_part_of_pair);
                        
                        //char c = getchar();
                        //merge_blocks (ptr + num_of_nums_in_part_of_pair*2*merged_pairs_num, num_of_nums_in_part_of_pair);
                        //qsort(ptr + nums_num * taked_block, nums_num, sizeof(struct index_s), compare_fun);
                        
                        //printf("\nThread [%d] sorted {%d} block\n", this_data.ind, taked_block);
                        //sleep(1);
                    }
                    else {
                        //printf("\nThread [%d] waites\n", this_data.ind);
                        pthread_mutex_unlock(&mutte);
                        break;
                    }
             }

             num_of_nums_in_part_of_pair *= 2; 



             //printf("\nThread [%d] waites on [%d] merge barr\n", this_data.ind, i);
            pthread_barrier_wait(&m_barrier);
            //printf("\nMain after {%d} merge barrier\n", i + 1);
        }



//}

    printf("\nThread [%d] is returned\n", this_data.ind);

    return NULL;//pthread_exit(NULL);
}


void merge_blocks (struct index_s * ptr, long int num) {

    struct index_s swap_place;
    long int ind1 = 0, ind2 = num, end_num = num * 2;
    //double num1, num2;

    while (ind1 < num && ind2 < end_num) {
        //num1 = ptr[ind1].time_mark; num2 = ptr[ind2].time_mark; 
        //int a = (int) (num1+num2);
        //num
        if (ptr[ind1].time_mark <= ptr[ind2].time_mark) {
            ind1++;
        }
        else {
            swap_place = ptr[ind2];
            for (int k = ind2; k > ind1; k--) {
                ptr[k] = ptr[k - 1];
            }
            ptr[ind1] = swap_place;
            ind1++;
            ind2++;
            num++;
        }

    }
    //printf("\nIts merged: [%ld][%ld][%ld]\n", ind1, ind2, num);

}



int count_nums_in_arr(int * arr, int count_num, int arr_size) {
    int countt = 0;
    for (int i = 0; i < arr_size; i++) {
        if (arr[i] == count_num) {
            countt++;
        }
    }
    return countt;
}




















    /*
    char * ptr = mmap(NULL, 600, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);//4096);

    //uint64_t * num_pt = (uint64_t *)ptr;
    //printf("\nNUM:{%ld}\n", *num_pt);

    //struct index_s * d_pt = (struct index_s *)(ptr + 8);
    struct index_s * d_pt = (struct index_s *)(ptr);
    printf("\nNUME:{%f}\n", d_pt[4].time_mark);


    munmap(ptr, 600); 

*/



    /*
    char * ptr = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);//4096);

    uint64_t * num_pt = (uint64_t *)ptr;
    printf("\nNUM:{%ld}\n", *num_pt);

    struct index_s * d_pt = (struct index_s *)(ptr + 8);
    printf("\nNUME:{%f}\n", d_pt->time_mark);


    munmap(ptr, 32); 
    */ 


/*
    printf("Page size:[%d]\nUint size:[%ld]", getpagesize(), sizeof(uint64_t));

    printf("fd1:[%d]\n", fd);
    //off_t offset = lseek(fd, 8, SEEK_SET);
    //printf("off:[%ld]  fd2:[%d]\n", offset, fd);
    printf("Page size:[%d]  LOng long:[%ld]\n", getpagesize(), sizeof(long long));

    uint64_t * ptr = mmap(NULL, 8, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


    printf("NUM:{%ld}\n", (* ptr));

    munmap(ptr, 8); 
*/



    /*
    //struct index_s * file_in_mem = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 8);
    void * file_in_mem = mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    struct index_s * ptr;
    ptr = (struct index_s *)((char *)file_in_mem + 8);

    printf("\n%f\n", (*ptr).time_mark);



    for (int i = 0; i < 5; i++) {
        printf("%f  ", ptr[i].time_mark);
    }

    printf("\n");
    ptr[3].time_mark = 43.666;

    for (int i = 0; i < 5; i++) {
        printf("%f  ", ptr[i].time_mark);
    }


    printf("\n");

    munmap(file_in_mem, sb.st_size); 
    */




    /*

    FILE * f = fopen(file_name, "rb");
    if (f == NULL) {
        fprintf(stderr, "File %s Opening ERROR", file_name);
        exit(1);
    }

    fread(&(records_info.recsords), sizeof(uint64_t), 1, f);
    printf("Num:[%ld]\n", records_info.recsords);


    for (int i = 0; i < 5; i++) {
        fread(&(ind_s[i]), sizeof(struct index_s), 1, f);
        printf("%f  ", ind_s[i].time_mark);
    }

    //struct index_s * a;
    //a->
    printf("\n");

    qsort(ind_s, 5, sizeof(struct index_s), compare_fun);
    for (int i = 0; i < 5; i++) {
        //fread(&(ind_s[i]), sizeof(struct index_s), 1, f);
        printf("%f  ", ind_s[i].time_mark);
    }
    printf("\n");

    fclose(f);
    */
