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

void task_set_description(Task *task, const char *description) {
    free(task->description);
    task->description = util_copy_string(description);
}

bool task_is_in_inbox(Task *task) {
    return task->project == NULL && !task->scheduled;
}

void task_schedule(Task *task, struct tm date) {
    task->scheduled = true;
    task->scheduled_for = date;
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

void task_free(Task *task) {
    if (task_get_project(task) != NULL) {
        project_remove(task_get_project(task), task);
    }
    free(task->description);
    free(task);
}
