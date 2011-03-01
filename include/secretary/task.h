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

    bool done;
} Task;

Task *task_new(int number, const char *description);

#define task_get_number(task) ((task)->number)
#define task_get_description(task) ((task)->description)
void task_set_description(Task *task, const char *description);
#define task_get_project(task) ((task)->project)

bool task_is_in_inbox(Task *task);


void task_schedule(Task *task, struct tm date);
#define task_get_scheduled_date(task) ((task)->scheduled_for)
void task_unschedule(Task *task);
bool task_is_scheduled(Task *task);
bool task_is_scheduled_for(Task *task, struct tm date);

#define task_mark_as_done(task) ((task)->done = true)
#define task_unmark_as_done(task) ((task)->done = false)
#define task_is_done(task) ((task)->done)

void task_free(Task *task);

#endif
