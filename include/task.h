#ifndef _TASK_H
# define _TASK_H

#include <stdlib.h>
#include <time.h>

struct Project;

typedef enum {
    INBOX, PROJECT, SCHEDULED
} TaskState;

typedef struct {
    int number;
    const char *description;
    struct Project *project;
    struct tm scheduled_for;
    TaskState state;
} Task;

Task *task_new(int number, const char *description);

#define task_get_number(task) ((task)->number)
#define task_get_description(task) ((task)->description)
#define task_get_project(task) ((task)->project)
#define task_get_state(task) ((task)->state)

void task_schedule(Task *task, struct tm date);
void task_unschedule(Task *task);
#define task_is_scheduled_for(task, date) (\
    ((task)->state == SCHEDULED) && \
    ((task)->scheduled_for.tm_mday == (date).tm_mday) && \
    ((task)->scheduled_for.tm_mon == (date).tm_mon) && \
    ((task)->scheduled_for.tm_year == (date).tm_year))

#define task_free(task) (free(task))

#endif
