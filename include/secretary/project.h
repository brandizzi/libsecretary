#ifndef _PROJECT_H
# define _PROJECT_H

#include <secretary/task.h>

#include <stdlib.h>

#define PROJECT_ARRAY_SIZE 128

typedef struct Project {
    char *name;

    Task *tasks[PROJECT_ARRAY_SIZE];
    int task_count;
} Project;

Project *project_new(const char *name);
void project_add_task(Project *project, Task *task);
void project_remove_task(Project *project, Task *task);

const char* project_get_name (Project *project);
void project_set_name(Project *project, const char *name);

int project_count_tasks (Project *project, bool archived);
Task *project_get_task(Project *project, int number);
Task *project_get_nth_task(Project *project, int n, bool archived);

void project_free(Project *project);

#endif
