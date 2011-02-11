#include <secretary/util.h>

#include <string.h>

char *util_copy_string(const char *string) {
    size_t size = strlen(string);
    char *new = malloc(sizeof(char)*(size+2));
    strncpy(new, string, size+1);
    return new;
}

char *util_read_string(FILE *file) {
    int length = getw(file);
    char *string = malloc((length+2)*sizeof(char));
    fgets(string, length+1, file);
    return string;
}
void util_write_string(FILE *file, const char *string) {
    putw(strlen(string), file);
    fputs(string, file);
}

