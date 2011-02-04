#ifndef _TASK_H
# define _TASK_H

#import <stdlib.h>

struct Project;

typedef struct {
    int number;
    const char *description;
    struct Project *project;  
} Task;

Task *task_new(int number, const char *description);

#define task_number(task) (task->number)
#define task_description(task) (task->description)
#define task_project(task) (task->project)
#define task_free(task) (free(task))

#endif
