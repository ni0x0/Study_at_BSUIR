
#include "hash_opts.h"

//char* file_name1;
//char* file_name2;


// Count and compare Files Hashes
int files_hash_compare(char* file_name1, char* file_name2) {

    EVP_MD_CTX *mdctx1;
    EVP_MD_CTX *mdctx2;
    const EVP_MD *md;

    unsigned char md1_value[EVP_MAX_MD_SIZE];
    unsigned char md2_value[EVP_MAX_MD_SIZE];
    unsigned int md1_len, md2_len;
    int are_equal;

    md = EVP_get_digestbyname("sha256");            // Getting hash type
        if (md == NULL) {
            fprintf(stderr, "\nEVP_get_digestbyname ERROR. Unknown message digest.\n");
            exit(1);
        }

    // Initialization
    mdctx1 = EVP_MD_CTX_new();
        if (!EVP_DigestInit_ex2(mdctx1, md, NULL)) {
            fprintf(stderr, "EVP_DigestInit_ex2 ERROR. Message 1 digest initialization failed.\n");
            EVP_MD_CTX_free(mdctx1);
            exit(1);
        }

    mdctx2 = EVP_MD_CTX_new();
        if (!EVP_DigestInit_ex2(mdctx2, md, NULL)) {
            fprintf(stderr, "EVP_DigestInit_ex2 ERROR. Message 2 digest initialization failed.\n");
            EVP_MD_CTX_free(mdctx1);
            EVP_MD_CTX_free(mdctx2);
            exit(1);
        }

    // Get info for hash
    file_digest_update(mdctx1, file_name1);
    file_digest_update(mdctx2, file_name2);

    // Get Final Hash Strings
    if (!EVP_DigestFinal_ex(mdctx1, md1_value, &md1_len)) {
        fprintf(stderr, "Message 1 digest finalization failed.\n");
        EVP_MD_CTX_free(mdctx1);
        EVP_MD_CTX_free(mdctx2);
        exit(1);
    }

    if (!EVP_DigestFinal_ex(mdctx2, md2_value, &md2_len)) {
        fprintf(stderr, "Message 2 digest finalization failed.\n");
        EVP_MD_CTX_free(mdctx1);
        EVP_MD_CTX_free(mdctx2);
        exit(1);
    }
    // Close MDs
    EVP_MD_CTX_free(mdctx1);
    EVP_MD_CTX_free(mdctx2);


    /*
    printf("Digest 1 is: ");
    for (unsigned int i = 0; i < md1_len; i++)
        printf("%02x ", md1_value[i]);

    printf("\nDigest 2 is: ");
    for (unsigned int i = 0; i < md2_len; i++)
        printf("%02x ", md2_value[i]);
    printf("\n");
    */

    int size_to_comp;
    if (md1_len > md2_len) {
        size_to_comp = md1_len;
    }
    else {
        size_to_comp = md2_len;
    }

    are_equal = !memcmp(md1_value, md2_value, size_to_comp);
    //printf("\nlen:%d\nmd1_len:%d\nmd2_len:%d\nare equal:%d\n", EVP_MAX_MD_SIZE, md1_len, md2_len, memcmp(md1_value, md2_value, size_to_comp));

    return are_equal;
}

// Get Hash info from file
void file_digest_update(EVP_MD_CTX *mdctx, char* fname) {

    // File Open
    char file_str[FILE_STR_READ_SYBMS];
    FILE * f_ptr;
    f_ptr = fopen(fname, "r");
    if (f_ptr != NULL) {
        // File view cycle
        //printf("{{");
        while (fgets(file_str, FILE_STR_READ_SYBMS, f_ptr)) {
            //printf("[%s]", file_str);
            digest_update(mdctx, file_str);
        }
        //printf("}}");

        fclose(f_ptr);
    }
    else {
        fprintf(stderr, "File \"%s\" Opening Problem", fname);
        exit (1);
    }


}

unsigned check_file_presense(char* file_name) {
    FILE* f = fopen(file_name, "r");
    if (f == NULL) {
        return 0;
    }
    else {
        fclose(f);
        return 1;
    }
}


// Update digest info
void digest_update(EVP_MD_CTX *mdctx, char* mess) {

    if (!EVP_DigestUpdate(mdctx, mess, strlen(mess))) {
        fprintf(stderr, "EVP_DigestUpdate ERROR. Message digest update failed.\n");
        EVP_MD_CTX_free(mdctx);
        exit(1);
    }

}



void link_create (char* old_path, char* new_path, char* log_f_name) {

    time_log(log_f_name);
    if (remove(new_path) == 0) {
        //printf("file {%s} is deleted.\n", new_path);
    } else {
        add_data_in_file(log_f_name, new_path);
        add_data_in_file(log_f_name, " deleting to creating hard link ERROR\n");
        //fprintf(stderr, "DElete file {%s} problem.\n", new_path);
        return;
    }

    if (link(old_path, new_path) == 0) {
        add_data_in_file(log_f_name, "Hard link on ");
        add_data_in_file(log_f_name, old_path);
        add_data_in_file(log_f_name, "\nby path ");
        add_data_in_file(log_f_name, new_path);
        add_data_in_file(log_f_name, " is created\n");
        //printf("HArd link {%s} is created\n", new_path);
    } else {
        add_data_in_file(log_f_name, "Hard link on ");
        add_data_in_file(log_f_name, old_path);
        add_data_in_file(log_f_name, "\nto ");
        add_data_in_file(log_f_name, new_path);
        add_data_in_file(log_f_name, " creating ERROR\n");
        //fprintf(stderr, "{%s} hard link creating ERROR\n", old_path);
        //return errno;
    }
}

void time_log(char* file_name) {

    time_t now = time(NULL);
    char time_str [TIME_STR_LEN] = "\n";
    strcpy(time_str + 1, ctime(&now));
    strcpy(time_str + strlen(time_str) - 1, ":\n");
    add_data_in_file(file_name, time_str);
}

void add_data_in_file(char* file_name, char* str_to_add) {
    FILE* f = fopen(file_name, "a");
    if (f == NULL) {
        fprintf(stderr, "%s open ERROR", file_name);
        exit(1);
    }
    fprintf(f, "%s", str_to_add);
    fclose(f);

}





/*
////////////////// MAIN /////////////////////////////
int main (int argc, char *argv[])
{

    // Get file name
    file_name1 = argv[1];
    file_name2 = argv[2];

    // For file statistic 
    struct stat sb;

    if (lstat(file_name1, &sb) == -1) {
        fprintf(stderr, "lstat ERROR");
        exit(EXIT_FAILURE);
    }

    printf("File type:");
    switch (sb.st_mode & S_IFMT) {
    case S_IFBLK: printf("block\n"); break;
      case S_IFCHR: printf("symbolic thing\n"); break;
      case S_IFDIR: printf("dir\n"); break;
      case S_IFIFO: printf("FIFO\n"); break;
      case S_IFLNK: printf("symbolic\n"); break;
      case S_IFREG: printf("usual file\n"); break;
      case S_IFSOCK: printf("soocket\n"); break;
      default: printf("idk?\n"); break;
    }

    printf("\nFile size: %ld bytes\n", sb.st_size);

    int are_equal = files_hash_compare();
    if (are_equal) {
        printf("\nThis are equal files\n");
    }
    else {
        printf("\nThis are not equal files\n");
    }

    exit(0);
}
*/

