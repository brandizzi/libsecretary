#include <secretary/task.h>
#include <secretary/project.h>
#include <secretary/util.h>
#include <stdlib.h>

#define SECONDS_IN_DAY (60*60*24)

Task *task_new(int number, const char *description) {
    Task *task = malloc(sizeof(Task));
    task->number = number;
    task->description = util_copy_string(description);
    task->project = NULL;
    task->scheduled = false;
    task->done = false;
    return task;
}

int task_get_number(Task *task) {
    return task->number;
}

const char *task_get_description(Task *task) {
    return task->description;
}

void task_set_description(Task *task, const char *description) {
    free(task->description);
    task->description = util_copy_string(description);
}

Project *task_get_project(Task *task) {
    return task->project;
}

void task_set_project(Task *task, Project *project) {
    project_add_task(project, task);
}

bool task_has_project(Task *task) {
    return task->project != NULL;
}

void task_unset_project(Task *task) {
    if (task_has_project(task) == NULL) return;
    project_remove_task(task->project, task);
}

bool task_is_in_inbox(Task *task) {
    return task->project == NULL && !task->scheduled;
}

void task_schedule(Task *task, struct tm date) {
    task->scheduled = true;
    task->scheduled_for = date;
}

struct date task_get_scheduled_date(Task *task) {
    return task->scheduled_for;
}

bool task_is_scheduled(Task *task) {
    return task->scheduled;
}

bool task_is_scheduled_for(Task *task, struct tm date) {
    time_t scheduled = mktime(&task->scheduled_for),
           compared = mktime(&date);
    long scheduled_date = scheduled/SECONDS_IN_DAY, // removing hours, minutes etc.
         compared_date = compared/SECONDS_IN_DAY;
    return task_is_scheduled(task) && scheduled_date <= compared_date;
}

void task_unschedule(Task *task) {
    task->scheduled = false;
}

void task_mark_as_done(Task *task) {
    task->done = true;
}

void task_unmark_as_done(Task *task) {
    task->done = false;
}
void task_is_done(Task *task) {
    return task->done;
}

void task_free(Task *task) {
    if (task_get_project(task) != NULL) {
        project_remove_task(task_get_project(task), task);
    }
    free(task->description);
    free(task);
}
