#include <secretary.h>

#include <stdlib.h>
#include <string.h>

Secretary *secretary_new() {
    return malloc(sizeof(Secretary));
}

void _secretary_delete(void *todelete, void **array, int *count) {
    int _count = *count;
    for (int i = 0; i < _count; i++) {
        void **cursor = array+i;
        if (*cursor == todelete) {
            memmove(cursor, cursor+1, (_count-i)*sizeof(void*));
            (*count)--;
            return;
        }
    }
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

void secretary_move(Secretary *secretary, Task *task, Project *project) {
    project_add(project, task);
    _secretary_delete(task, (void**)secretary->inbox, &(secretary->inbox_count));
}

void secretary_move_to_inbox(Secretary *secretary, Task *task) {
    Project *project = task_project(task);
    project_remove(project, task);
    secretary->inbox[secretary->inbox_count++] = task;
}

void secretary_delete_task(Secretary *secretary, Task *task) {
    Project *project = task_project(task);
    if (project) {
        project_remove(project, task);
    } else {
        _secretary_delete(task, (void**)secretary->inbox, &(secretary->inbox_count));
    }
    _secretary_delete(task, (void**)secretary->tasks, &(secretary->task_count));
    task_free(task);
}

void secretary_delete_project(Secretary *secretary, Project *project) {
    int tn = project_task_count(project);
    for (int i = 0; i < tn; i++) {
        secretary_move_to_inbox(secretary, project_get_nth_task(project, i));
    }
    _secretary_delete(project, (void**)secretary->projects, &(secretary->project_count));
    project_free(project);
}

