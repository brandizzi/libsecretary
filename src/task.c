#include <secretary/task.h>
#include <secretary/util.h>
#include <stdlib.h>

Task *task_new(int number, const char *description) {
    Task *task = malloc(sizeof(Task));
    task->number = number;
    task->description = util_copy_string(description);
    task->project = NULL;
    task->scheduled = false;
    task->done = false;
    return task;
}

bool task_is_in_inbox(Task *task) {
    return task->project == NULL && !task->scheduled && !task->done;
}

void task_schedule(Task *task, struct tm date) {
    task->scheduled = true;
    task->scheduled_for = date;
}

bool task_is_scheduled(Task *task) {
    return task->scheduled && !task->done;
}

bool task_is_scheduled_for(Task *task, struct tm date) {
    return task_is_scheduled(task) &&
        (task->scheduled_for.tm_mday == date.tm_mday) &&
        (task->scheduled_for.tm_mon == date.tm_mon) && 
        (task->scheduled_for.tm_year == date.tm_year);
}

void task_unschedule(Task *task) {
    task->scheduled = false;
}

void task_free(Task *task) {
    free(task->description);
    free(task);
}
