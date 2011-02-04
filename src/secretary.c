#include <secretary.h>

#include <stdlib.h>

Secretary *secretary_new() {
    return malloc(sizeof(Secretary));
}

Task *secretary_appoint(Secretary *secretary, const char* description) {
    Task *task = task_new(++secretary->task_count, description);
    secretary->tasks[secretary->task_count-1] = task;
    secretary->inbox[secretary->inbox_count++] = task;
    return task;
}

