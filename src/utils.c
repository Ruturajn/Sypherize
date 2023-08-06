#include "../inc/utils.h"

const char *err_strings[] = {"GENERIC",   "ARGS",   "MEMORY ALLOCATION", "FILE ACCESS", "FILE SIZE",
                             "FILE READ", "SYNTAX", "REDEFINITION",      "EOF",         "TYPE"};

char *read_file_data(char *file_dest) {
    FILE *file_ptr = NULL;
    file_ptr = fopen(file_dest, "r");
    if (file_ptr == NULL)
        print_error(ERR_FILE_OPEN, "Unable to open file : `%s`", file_dest, 0);

    size_t file_sz = calculate_file_size(file_ptr);

    char *file_data = NULL;

    file_data = (char *)calloc((file_sz + 1), sizeof(char));
    CHECK_NULL(file_data, "Unable to allocate memory for file contents", NULL);

    size_t bytes_read = fread(file_data, 1, file_sz, file_ptr);

    if (bytes_read != file_sz)
        print_error(ERR_FILE_READ, "Unable to read file contents : `%s`", file_dest, 0);

    file_data[file_sz] = '\0';
    fclose(file_ptr);
    return file_data;
}

void print_error(ErrType err, char *fmt, char *str, int val) {
    printf("\033[1;31m[ERROR]\033[1;37m ");
    printf("%s :: ", err_strings[err]);
    if (val != 0)
        printf((const char *)fmt, val);
    else {
        if (str != NULL)
            printf((const char *)fmt, str);
        else
            printf("%s", fmt);
    }
    printf("!\n");
    exit(EXIT_FAILURE);
}

void print_warning(ErrType err, char *fmt, char *str, int val) {
    printf("\033[1;32m[WARN]\033[1;37m ");
    printf("%s :: ", err_strings[err]);
    if (val != 0)
        printf((const char *)fmt, val);
    else {
        if (str != NULL)
            printf((const char *)fmt, str);
        else
            printf("%s", fmt);
    }
    printf("!\n");
}

size_t calculate_file_size(FILE *file_ptr) {
    if (file_ptr == NULL) {
        return 0;
    }

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error(ERR_FILE_OPEN, "Could not open source file", NULL, 0);

    if (fseek(file_ptr, 0, SEEK_END) < 0)
        print_error(ERR_FILE_OPEN, "Could not open source file", NULL, 0);

    long file_sz = ftell(file_ptr);

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error(ERR_FILE_OPEN, "Could not open source file", NULL, 0);

    if (file_sz < 0)
        print_error(ERR_FILE_SIZE, "Could not calculate file size", NULL, 0);

    return file_sz;
}
