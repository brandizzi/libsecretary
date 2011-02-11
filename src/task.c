#include <secretary/task.h>
#include <secretary/util.h>
#include <stdlib.h>

Task *task_new(int number, const char *description) {
    Task *task = malloc(sizeof(Task));
    task->number = number;
    task->description = util_copy_string(description);
    task->project = NULL;
    task->scheduled = false;
    return task;
}

bool task_is_in_inbox(Task *task) {
    return task->project == NULL && !task->scheduled;
}

void task_schedule(Task *task, struct tm date) {
    task->scheduled = true;
    task->scheduled_for = date;
}

void task_unschedule(Task *task) {
    task->scheduled = false;
}

void task_free(Task *task) {
    free(task->description);
    free(task);
}
