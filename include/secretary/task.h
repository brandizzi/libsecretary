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

int task_get_number(Task *task);
const char *task_get_description(Task *task);
void task_set_description(Task *task, const char *description);

struct Project *task_get_project(Task *task);
void task_set_project(Task *task, struct Project *project);
void task_unset_project(Task *task);
bool task_has_project(Task *task);
bool task_is_in_project(Task *task, struct Project *project);

bool task_is_in_inbox(Task *task);


void task_schedule(Task *task, struct tm date);
struct tm task_get_scheduled_date(Task *task);
void task_unschedule(Task *task);
bool task_is_scheduled(Task *task);
bool task_is_scheduled_for(Task *task, struct tm date);

void task_mark_as_done(Task *task);
void task_unmark_as_done(Task *task);
void task_switch_done_status(Task *task);
bool task_is_done(Task *task);

void task_free(Task *task);

#endif
