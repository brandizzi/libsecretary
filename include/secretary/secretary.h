#ifndef _SECRETARY_H
# define _SECRETARY_H

#include <secretary/task.h>
#include <secretary/project.h>
#include <time.h>

#define ARRAY_SIZE 1024

typedef struct {
    Task *tasks[ARRAY_SIZE];
    int task_count;

    Project *projects[ARRAY_SIZE];
    int project_count;
    
} Secretary;

Secretary *secretary_new();
Task *secretary_create_task(Secretary *secretary, const char* description);
int secretary_count_tasks(Secretary *secretary);
Task *secretary_get_task(Secretary *secretary, int number);
Task *secretary_get_nth_task(Secretary *secretary, int n);

int secretary_count_inbox_tasks(Secretary *secretary);
Task *secretary_get_nth_inbox_task(Secretary *secretary, int n);

Project *secretary_create_project(Secretary *secretary, const char* name);
int secretary_count_projects(Secretary *secretary);
Project *secretary_get_project(Secretary *secretary, const char *name);
Project *secretary_get_nth_project(Secretary *secretary, int n);

int secretary_count_tasks_scheduled(Secretary *secretary);
int secretary_count_tasks_scheduled_for(Secretary *secretary, struct tm date);
int secretary_count_tasks_scheduled_for_today(Secretary *secretary);
Task *secretary_get_nth_task_scheduled(Secretary *secretary, int n);
Task *secretary_get_nth_task_scheduled_for(Secretary *secretary, struct tm date, int n);
Task *secretary_get_nth_task_scheduled_for_today(Secretary *secretary, int n);

int secretary_count_done_tasks(Secretary *secretary);
Task *secretary_get_nth_done_task(Secretary *secretary, int n);

void secretary_delete_task(Secretary *secretary, Task *task);
void secretary_delete_project(Secretary *secretary, Project *project);

void secretary_free(Secretary *secretary);

#endif
