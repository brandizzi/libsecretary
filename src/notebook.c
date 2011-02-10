#include <secretary/notebook.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 2048
#define TASK_HAS_PROJECT 0X1

static const char *_notebook_read_string(FILE *file);
static void _notebook_write_string(FILE *file, const char *string);

Notebook *notebook_new(const char *filename) {
    Notebook *notebook = malloc(sizeof(Notebook));
    Secretary *secretary = notebook->secretary = secretary_new();    
    notebook->filename = filename;

    FILE *file = fopen(filename, "r");
    if (file) {
        notebook->major_version = getc(file);
        notebook->minor_version = getc(file);
        int project_count = getw(file);
        for (int i = 0; i < project_count; i++) {
            //int properties = 
                getw(file);
            const char *name = _notebook_read_string(file);
            secretary_start(secretary, name);
        }

        int task_count = getw(file);
        for (int i = 0; i < task_count; i++) {
            int properties = getw(file);
            const char *description = _notebook_read_string(file);
            Task *task = secretary_appoint(secretary, description);
            if (properties & TASK_HAS_PROJECT) {
                const char *name  = _notebook_read_string(file);
                Project *project = secretary_get_project(secretary, name);
                secretary_move(secretary, task, project);
            }
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
    Secretary *secretary = notebook_get_secretary(notebook);
    putc(notebook->major_version, file);
    putc(notebook->minor_version, file);
    putw(secretary_count_project(secretary), file);
    for (int i = 0; i < secretary_count_project(secretary); i++) {
        Project *project = secretary_get_nth_project(secretary, i);
        int mask = 0;
        putw(mask, file);
        _notebook_write_string(file, project_get_name(project));
    }
    putw(secretary_count_task(secretary), file);
    for (int i = 0; i < secretary_count_inbox(secretary); i++) {
        Task *task = secretary_get_nth_inbox_task(secretary, i);
        int mask = 0;
        putw(mask, file);
        _notebook_write_string(file, task_get_description(task));
    }
    // Saving tasks from projects
    for (int i = 0; i < secretary_count_project(secretary); i++) {
        Project *project = secretary_get_nth_project(secretary, i);
        for (int j = 0; j < project_count_task(project); j++) {
            Task *task = project_get_nth_task(project, j);
            int mask = TASK_HAS_PROJECT;
            putw(mask, file);
            _notebook_write_string(file, task_get_description(task));
            _notebook_write_string(file, 
                    project_get_name(task_get_project(task)));
        }
    }
    fclose(file);
}

void notebook_free(Notebook *notebook) {
    secretary_free(notebook_get_secretary(notebook));
    free(notebook);
}

static const char *_notebook_read_string(FILE *file) {
    int length = getw(file);
    char *string = malloc((length+2)*sizeof(char));
    fgets(string, length+1, file);
    return string;
}

static void _notebook_write_string(FILE *file, const char *string) {
    putw(strlen(string), file);
    fputs(string, file);
}


