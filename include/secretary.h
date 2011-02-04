#ifndef _SECRETARY_H
# define _SECRETARY_H

#include <task.h>
#include <project.h>

#define ARRAY_SIZE 1024

typedef struct {
    Task *tasks[ARRAY_SIZE];
    int task_count;

    Task *inbox[ARRAY_SIZE];
    int inbox_count;

    Project *projects[ARRAY_SIZE];
    int project_count;
    
} Secretary;

Secretary *secretary_new();
Task *secretary_appoint(Secretary *secretary, const char* description);
Project *secretary_start(Secretary *secretary, const char* name);
Project *secretary_get_project(Secretary *secretary, const char *name);
void secretary_move(Secretary *secretary, Task *task, Project *project);
void secretary_move_to_inbox(Secretary *secretary, Task *task);
void secretary_delete_task(Secretary *secretary, Task *task);
void secretary_delete_project(Secretary *secretary, Project *project);

/* Accessors */
#define secretary_get_task_count(secretary) ((secretary)->task_count)
#define secretary_get_project_count(secretary) ((secretary)->project_count)
#define secretary_get_inbox_count(secretary) ((secretary)->inbox_count)
#define secretary_get_area_count(secretary) 0
#define secretary_get_task(secretary, number) (((secretary)->task_count>=(number))? (secretary)->tasks[(number)-1] : NULL )
#define secretary_get_inbox_task(secretary, number) (((secretary)->inbox_count>=(number))? (secretary)->inbox[(number)-1] : NULL )


#endif
