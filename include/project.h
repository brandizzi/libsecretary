#ifndef _PROJECT_H
# define _PROJECT_H

#include <task.h>

#include <stdlib.h>

#define PROJECT_ARRAY_SIZE 128

typedef struct Project {
    const char *name;

    Task *tasks[PROJECT_ARRAY_SIZE];
    int task_count;
} Project;

Project *project_new(const char *name);
void project_add(Project *project, Task *task);
void project_remove(Project *project, Task *task);
Task *project_get_task(Project *project, int number);
void project_free(Project *project);

#define project_get_name(project) ((project)->name)
#define project_count_task(project) ((project)->task_count)
#define project_get_nth_task(project, n) ((project)->tasks[(n)])

#endif
