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
#define secretary_count_tasks(secretary) ((secretary)->task_count)
Task *secretary_get_task(Secretary *secretary, int number);
#define secretary_get_nth_task(secretary, n) ((secretary)->tasks[n])

int secretary_count_inbox_tasks(Secretary *secretary);
Task *secretary_get_nth_inbox_task(Secretary *secretary, int n);

Project *secretary_create_project(Secretary *secretary, const char* name);
#define secretary_count_projects(secretary) ((secretary)->project_count)
Project *secretary_get_project(Secretary *secretary, const char *name);
#define secretary_get_nth_project(secretary, n) \
        (((secretary)->project_count > (n))? ((secretary)->projects[n]) : NULL)

void secretary_move_to_project(Secretary *secretary, Task *task, Project *project);
void secretary_remove_from_project(Secretary *secretary, Task *task);

void secretary_delete_task(Secretary *secretary, Task *task);
void secretary_delete_project(Secretary *secretary, Project *project);

void secretary_schedule(Secretary *secretary, Task *task, struct tm date);
int secretary_count_tasks_scheduled(Secretary *secretary);
int secretary_count_tasks_scheduled_for(Secretary *secretary, struct tm date);
int secretary_count_tasks_scheduled_for_today(Secretary *secretary);
Task *secretary_get_nth_task_scheduled(Secretary *secretary, int n);
Task *secretary_get_nth_task_scheduled_for(Secretary *secretary, struct tm date, int n);
Task *secretary_get_nth_task_scheduled_for_today(Secretary *secretary, int n);
void secretary_unschedule_task(Secretary *secretary, Task *task);

#define secretary_mark_task_as_done(secretary, task) (task_mark_as_done(task))
#define secretary_unmark_task_as_done(secretary, task) (task_unmark_as_done(task))
int secretary_count_done_tasks(Secretary *secretary);
Task *secretary_get_nth_done_task(Secretary *secretary, int n);

void secretary_free(Secretary *secretary);

#define secretary_count_area(secretary) 0

#endif
