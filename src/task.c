#include <task.h>

#include <stdlib.h>

Task *task_new(int number, const char *description) {
    Task *task = malloc(sizeof(Task));
    task->number = number;
    task->description = description;
    task->project = NULL;
    task->state = INBOX;
    return task;
}

