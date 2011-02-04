#include <secretary.h>

#include <stdlib.h>
#include <string.h>

Secretary *secretary_new() {
    return malloc(sizeof(Secretary));
}

Task *secretary_appoint(Secretary *secretary, const char* description) {
    Task *task = task_new(++secretary->task_count, description);
    secretary->tasks[secretary->task_count-1] = task;
    secretary->inbox[secretary->inbox_count++] = task;
    return task;
}

Project *secretary_start(Secretary *secretary, const char* name) {
    Project *project = project_new(name);
    secretary->projects[secretary->project_count++] = project;
    return project;
}

Project *secretary_get_project(Secretary *secretary, const char *name) {
    for (int i = 0; i < secretary->project_count; i++) {
        Project *project = secretary->projects[i];
        if (strcmp(name, project_name(project)) == 0) {
            return project;
        }
    }
    
    return NULL;
}

