#include "../inc/lexer.h"
#include <string.h>

long calculate_file_size(FILE *file_ptr) {

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error("Unable to access file");

    if (fseek(file_ptr, 0, SEEK_END) < 0)
        print_error("Unable to access file");

    long file_sz = ftell(file_ptr);

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error("Unable to access file");

    if (file_sz < 0)
        print_error("Unable to get file size");

    printf("The file size is : %ld\n", file_sz);

    return file_sz;
}

void lex_file(char *file_dest) {

    FILE *file_ptr = NULL;
    file_ptr = fopen(file_dest, "r");
    CHECK_NULL(file_ptr, "Unable to open file");

    long file_sz = calculate_file_size(file_ptr);

    char *file_data = NULL;

    file_data = (char *)malloc((file_sz + 1) * sizeof(char));
    CHECK_NULL(file_data, "Unable to allocate memory");

    size_t bytes_read = fread(file_data, 1, file_sz, file_ptr);

    if (bytes_read != file_sz)
        print_error("Unable to read from file");

    file_data[file_sz] = '\0';

    printf("---\n%s\n---\n", file_data);

    /// Tokenizing;
    char delims[20] = " :=,;~()\r\n";
    char whitespace[5] = " \r\n";

    int begin = 0;
    int end = 0;
    char *temp_file_data = file_data;
    begin = strspn(temp_file_data, whitespace);
    temp_file_data += begin;

    while (*temp_file_data != '\0') {
        begin = strcspn(temp_file_data, delims);
        if (begin < 1) {
            if (*temp_file_data != ' ')
                printf("%.*s\n", (int)(1), temp_file_data);
            temp_file_data +=1 ;
        }
        else
            printf("%.*s\n", begin, temp_file_data);
        end = strspn(temp_file_data, whitespace);
        temp_file_data += (begin + end);
    }

    free(file_data);
    fclose(file_ptr);
}
