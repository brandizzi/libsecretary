#include <secretary/secretary.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void _secretary_delete(void *todelete, void **array, int *count);

Secretary *secretary_new() {
    Secretary *secretary = malloc(sizeof(Secretary));
    secretary->task_count = 0;
    secretary->project_count = 0;
    return secretary;
}

Task *secretary_create_task(Secretary *secretary, const char* description) {
    Task *task = task_new(++secretary->task_count, description);
    secretary->tasks[secretary->task_count-1] = task;
    return task;
}

Project *secretary_create_project(Secretary *secretary, const char* name) {
    Project *project = project_new(name);
    secretary->projects[secretary->project_count++] = project;
    return project;
}

Project *secretary_get_project(Secretary *secretary, const char *name) {
    for (int i = 0; i < secretary->project_count; i++) {
        Project *project = secretary->projects[i];
        if (strcmp(name, project_get_name(project)) == 0) {
            return project;
        }
    }
    
    return NULL;
}

void secretary_move_to_project(Secretary *secretary, Task *task, Project *project) {
    Project *origin = task_get_project(task);
    if (origin != NULL) {
        project_remove_task(origin, task);
    }
    project_add_task(project, task);
}

void secretary_move_to_project_to_inbox(Secretary *secretary, Task *task) {
    Project *project = task_get_project(task);
    if (project) {
        project_remove_task(project, task);
    }
}

void secretary_delete_task(Secretary *secretary, Task *task) {
    Project *project = task_get_project(task);
    if (project) {
        project_remove_task(project, task);
    }
    _secretary_delete(task, (void**)secretary->tasks, &(secretary->task_count));
    task_free(task);
}

void secretary_delete_project(Secretary *secretary, Project *project) {
    int tn = project_count_tasks(project);
    for (int i = 0; i < tn; i++) {
        secretary_move_to_project_to_inbox(secretary, project_get_nth_task(project, i));
    }
    _secretary_delete(project, (void**)secretary->projects, &(secretary->project_count));
    project_free(project);
}

int secretary_count_inbox_tasks(Secretary *secretary) {
    int counter = 0;
    for (int i = 0; i < secretary->task_count; i++) {
        if (task_is_in_inbox(secretary->tasks[i])) counter++;
    }
    return counter;
}

Task *secretary_get_nth_inbox_task(Secretary *secretary, int n) {
    for (int i = 0; i < secretary->task_count; i++) {
        if (task_is_in_inbox(secretary->tasks[i])) {
            if (n-- == 0) return secretary->tasks[i];
        }
    }
    return NULL;
}

Task *secretary_get_task(Secretary *secretary, int number) {
    for (int i = 0; i < secretary->task_count; i++) {
        if (task_get_number(secretary->tasks[i]) == number) {
           return secretary->tasks[i];
        }
    }
    return NULL;
}

void secretary_free(Secretary *secretary) {
    for (int i = 0; i < secretary->project_count; i++) {
        project_free(secretary->projects[i]);
    }
    for (int i = 0; i < secretary->task_count; i++) {
        task_free(secretary->tasks[i]);
    }
    free(secretary);
}

void secretary_schedule(Secretary *secretary, Task *task, struct tm date) {
    task_schedule(task, date);
}

int secretary_count_tasks_scheduled(Secretary *secretary) {
    int counter = 0;
    for (int i = 0; i < secretary->task_count; i++) {
        if (task_is_scheduled(secretary->tasks[i])) counter++;
    }
    return counter;
}


int secretary_count_tasks_scheduled_for(Secretary *secretary, struct tm date) {
    int counter = 0;
    for (int i = 0; i < secretary->task_count; i++) {
        if (task_is_scheduled_for(secretary->tasks[i], date)) {
           counter++;
        }
    }
    return counter;
}
int secretary_count_tasks_scheduled_for_today(Secretary *secretary) {
    time_t now = time(NULL);
    return secretary_count_tasks_scheduled_for(secretary, *localtime(&now));
}

Task *secretary_get_nth_task_scheduled(Secretary *secretary, int n) {
    for (int i = 0; i < secretary->task_count; i++) {
        if (task_is_scheduled(secretary->tasks[i])) {
            if (n-- == 0) return secretary->tasks[i];
        }
    }
    return NULL;
}

Task *secretary_get_nth_task_scheduled_for(Secretary *secretary, struct tm date, int n) {
    for (int i = 0; i < secretary->task_count; i++) {
        Task *task = secretary->tasks[i];
        if (task_is_scheduled_for(task, date)) {
            if (n-- == 0) return task;
        }
    }
    return NULL;
}
Task *secretary_get_nth_task_scheduled_for_today(Secretary *secretary, int n) {
    time_t now = time(NULL);
    return secretary_get_nth_task_scheduled_for(secretary, *localtime(&now), n);
}

void secretary_unschedule_task(Secretary *secretary, Task *task) {
    task_unschedule(task);
}

int secretary_count_done_tasks(Secretary *secretary) {
    int counter = 0;
    for (int i = 0; i < secretary->task_count; i++) {
        if (task_is_done(secretary->tasks[i])) counter++;
    }
    return counter;
}

Task *secretary_get_nth_done_task(Secretary *secretary, int n) {
    for (int i = 0; i < secretary->task_count; i++) {
        if (task_is_done(secretary->tasks[i])) {
            if (n-- == 0) return secretary->tasks[i];
        }
    }
    return NULL;
}

// PRIVATE STUFF
static void _secretary_delete(void *todelete, void **array, int *count) {
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


