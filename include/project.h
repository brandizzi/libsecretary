#ifndef _PROJECT_H
# define _PROJECT_H

#include <task.h>

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

#define project_name(project) ((project)->name)
#define project_task_count(project) ((project)->task_count)

#endif
