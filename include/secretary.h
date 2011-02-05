#ifndef _SECRETARY_H
# define _SECRETARY_H

#include <task.h>
#include <project.h>
#include <time.h>

#define ARRAY_SIZE 1024

typedef struct {
    Task *tasks[ARRAY_SIZE];
    int task_count;

    Project *projects[ARRAY_SIZE];
    int project_count;
    
} Secretary;

Secretary *secretary_new();
Task *secretary_appoint(Secretary *secretary, const char* description);
#define secretary_get_task_count(secretary) ((secretary)->task_count)
Task *secretary_get_task(Secretary *secretary, int number);

int secretary_get_inbox_count(Secretary *secretary);
Task *secretary_get_nth_inbox_task(Secretary *secretary, int n);

Project *secretary_start(Secretary *secretary, const char* name);
#define secretary_get_project_count(secretary) ((secretary)->project_count)
Project *secretary_get_project(Secretary *secretary, const char *name);

void secretary_move(Secretary *secretary, Task *task, Project *project);
void secretary_move_to_inbox(Secretary *secretary, Task *task);

void secretary_delete_task(Secretary *secretary, Task *task);
void secretary_delete_project(Secretary *secretary, Project *project);

void secretary_schedule(Secretary *secretary, Task *task, struct tm date);
int secretary_count_scheduled_for(Secretary *secretary, struct tm date);
int secretary_count_scheduled_for_today(Secretary *secretary);
Task *secretary_get_nth_scheduled_for(Secretary *secretary, struct tm date, int n);
Task *secretary_get_nth_scheduled_for_today(Secretary *secretary, int n);
void secretary_unschedule(Secretary *secretary, Task *task);

void secretary_free(Secretary *secretary);

#define secretary_get_area_count(secretary) 0

#endif
