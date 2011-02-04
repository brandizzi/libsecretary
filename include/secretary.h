#ifndef _SECRETARY_H
# define _SECRETARY_H

#include <task.h>

typedef struct {
    Task *tasks[1024];
    int task_count;

    Task *inbox[1024];
    int inbox_count;
    
} Secretary;

Secretary *secretary_new();
#define secretary_task_count(secretary) (secretary->task_count)
#define secretary_project_count(secretary) 0
#define secretary_inbox_count(secretary) (secretary->inbox_count)
#define secretary_area_count(secretary) 0

Task *secretary_appoint(Secretary *secretary, const char* description);

#endif
