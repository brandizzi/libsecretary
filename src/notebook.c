#include <secretary/notebook.h>
#include <secretary/util.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Notebook *notebook_new(const char *filename) {
    Notebook *notebook = malloc(sizeof(Notebook));
    Secretary *secretary = notebook->secretary = secretary_new();    
    notebook->filename = filename;

    FILE *file = fopen(filename, "r");
    if (file) {
        notebook->major_version = getc(file);
        notebook->minor_version = getc(file);
        ParserFunction parser = parser_get(notebook->major_version, notebook->minor_version);
        notebook->secretary = parser(file);
    } else {
        notebook->major_version = PARSER_LATEST_MAJOR_VERSION;
        notebook->minor_version = PARSER_LATEST_MINOR_VERSION;

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
        util_write_string(file, project_get_name(project));
    }
    putw(secretary_count_task(secretary), file);
    for (int i = 0; i < secretary_count_task(secretary); i++) {
        Task *task = secretary_get_nth_task(secretary, i);
        int mask = 0;
        if (task_get_project(task)) {
            mask |= TASK_HAS_PROJECT;
        }
        if (task_is_scheduled(task)) {
            mask |= TASK_IS_SCHEDULED;
        }
        if (task_is_done(task)) {
            mask |= TASK_IS_DONE;
        }
        putw(mask, file);
        util_write_string(file, task_get_description(task));

        if (mask & TASK_HAS_PROJECT) {
            util_write_string(file, 
                    project_get_name(task_get_project(task)));
        }
        if (mask & TASK_IS_SCHEDULED) {
            struct tm date = task_get_scheduled_date(task);
            fwrite(&date, sizeof(date), 1, file);
        }
    }
    fclose(file);
}

void notebook_free(Notebook *notebook) {
    secretary_free(notebook_get_secretary(notebook));
    free(notebook);
}


