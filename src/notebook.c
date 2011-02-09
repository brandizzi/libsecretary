#include <secretary/notebook.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 2048

Notebook *notebook_new(const char *filename) {
    Notebook *notebook = malloc(sizeof(Notebook));
    notebook->secretary = secretary_new();    
    notebook->filename = filename;

    FILE *file = fopen(filename, "r");
    if (file) {
        notebook->major_version = getc(file);
        notebook->minor_version = getc(file);
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer));
        while (!feof(file)) {
            getc(file); // Getting task state
            fgets(buffer, BUFFER_SIZE, file);
            int len = strlen(buffer);
            // TODO how to free the strings?!
            char *description = malloc(sizeof(char)*len);
            // Make better
            char *cursor = buffer;
            cursor = strsep(&cursor, "\1");
            strncpy(description, cursor, len);
            secretary_appoint(notebook->secretary, description);
        }
        fclose(file);
    } else {
        notebook->major_version = 1;
        notebook->minor_version = 1;

    }
    return notebook;
}

void notebook_save(Notebook *notebook) {
    FILE *file = fopen(notebook->filename, "w");
    putc(notebook->major_version, file);
    putc(notebook->minor_version, file);
    for (int i = 0; i < secretary_count_inbox(notebook->secretary); i++) {
        Task *task = secretary_get_nth_inbox_task(notebook->secretary, i);
        putc(task->state, file);
        fprintf(file, "%s\1", task->description);
    }
    fclose(file);
}

void notebook_free(Notebook *notebook) {
    secretary_free(notebook_get_secretary(notebook));
    free(notebook);
}

