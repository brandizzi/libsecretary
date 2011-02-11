#ifndef _TASK_H
# define _TASK_H

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

struct Project;

typedef struct {
    int number;
    char *description;
    struct Project *project;

    bool scheduled;
    struct tm scheduled_for;
} Task;

Task *task_new(int number, const char *description);

#define task_get_number(task) ((task)->number)
#define task_get_description(task) ((task)->description)
#define task_get_project(task) ((task)->project)
#define task_get_scheduled_date(task) ((task)->scheduled_for);
bool task_is_in_inbox(Task *task);

void task_schedule(Task *task, struct tm date);
void task_unschedule(Task *task);
#define task_is_scheduled(task) ((task)->scheduled)
#define task_is_scheduled_for(task, date) (\
    task_is_scheduled(task) && \
    ((task)->scheduled_for.tm_mday == (date).tm_mday) && \
    ((task)->scheduled_for.tm_mon == (date).tm_mon) && \
    ((task)->scheduled_for.tm_year == (date).tm_year))

void task_free(Task *task);

#endif
