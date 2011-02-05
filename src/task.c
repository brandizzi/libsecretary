#include <secretary/task.h>

#include <stdlib.h>

Task *task_new(int number, const char *description) {
    Task *task = malloc(sizeof(Task));
    task->number = number;
    task->description = description;
    task->project = NULL;
    task->state = INBOX;
    return task;
}

void task_schedule(Task *task, struct tm date) {
    task->state = SCHEDULED;
    task->scheduled_for = date;
}

void task_unschedule(Task *task) {
    task->state = task->project? PROJECT : INBOX;
}

