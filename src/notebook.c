#include <secretary/notebook.h>
#include <secretary/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 2048
#define TASK_HAS_PROJECT 0x1
#define TASK_IS_SCHEDULED 0x2
#define TASK_IS_DONE 0x4

Notebook *notebook_new(const char *filename) {
    Notebook *notebook = malloc(sizeof(Notebook));
    notebook->filename = filename;

    FILE *file = fopen(filename, "r");
    if (file) {
        notebook->major_version = getc(file);
        notebook->minor_version = getc(file);
        ParserReaderFunction reader = parser_get_reader(
                notebook->major_version, notebook->minor_version);
        notebook->secretary = reader(file);
    } else {
        notebook->major_version = PARSER_LATEST_MAJOR_VERSION;
        notebook->minor_version = PARSER_LATEST_MINOR_VERSION;
        notebook->secretary = secretary_new();

    }
    return notebook;
}

void notebook_save(Notebook *notebook) {
    FILE *file = fopen(notebook->filename, "w");
    Secretary *secretary = notebook_get_secretary(notebook);
    putc(notebook->major_version, file);
    putc(notebook->minor_version, file);
    ParserWriterFunction writer = parser_get_writer(
            notebook->major_version, notebook->minor_version);
    writer(notebook->secretary, file);
    fclose(file);
}

void notebook_free(Notebook *notebook) {
    secretary_free(notebook_get_secretary(notebook));
    free(notebook);
}


