#include "../inc/helpers.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        print_error("Invalid number of arguments");
    }

    char *file_dest = argv[1];
    FILE *file_ptr = fopen(file_dest, "r");
    CHECK_NULL(file_ptr, "Unable to open file");

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error("Unable to access file");

    if (fseek(file_ptr, 0, SEEK_END) < 0)
        print_error("Unable to access file");

    long int file_sz = ftell(file_ptr);

    if (fseek(file_ptr, 0, SEEK_SET) < 0)
        print_error("Unable to access file");

    if (file_sz < 0)
        print_error("Unable to get file size");

    printf("The file size is : %ld\n", file_sz);

    char *file_data = NULL;
    file_data = (char *)malloc(file_sz * sizeof(char));
    CHECK_NULL(file_data, "Unable to allocate memory");

    size_t bytes_read = fread(file_data, 1, file_sz, file_ptr);

    if (bytes_read != file_sz)
        print_error("Unable to read from file");

    printf("---\n%s\n---\n", file_data);

    /// Tokenizing;
    char delims[15] = " :=;\r\n->(){}#";

    char *token = strtok(file_data, delims);

    while (token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, delims);
    }

    free(file_data);
    free(token);

    exit(EXIT_SUCCESS);
}
