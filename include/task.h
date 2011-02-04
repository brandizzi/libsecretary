#ifndef _TASK_H
# define _TASK_H

#include <task.h>

typedef struct {
    int number;
    const char *description;    
} Task;

Task *task_new(int number, const char *description);

#define task_number(task) (task->number)
#define task_description(task) (task->description)

#endif
