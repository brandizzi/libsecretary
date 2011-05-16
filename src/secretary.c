#include <secretary/secretary.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

Secretary *secretary_new() {
    Secretary *secretary = calloc(1, sizeof(Secretary));
    secretary->tasks = list_new();
    secretary->projects = list_new();
    secretary->inbox_perspective.visible_tasks = list_new();
    secretary->inbox_perspective.archived_tasks = list_new();
    secretary->scheduled_perspective.visible_tasks = list_new();
    secretary->scheduled_perspective.archived_tasks = list_new();
    return secretary;
}

Task *secretary_create_task(Secretary *secretary, const char* description) {
    Task *task = task_new(list_count_items(secretary->tasks)+1, description);
    task->secretary = secretary;
    list_add_item(secretary->tasks, task);
    // Registering in inbox
    list_add_item(secretary->inbox_perspective.visible_tasks, task);
    return task;
}

int secretary_count_tasks(Secretary *secretary) {
    return list_count_items(secretary->tasks);
}

Task *secretary_get_nth_task(Secretary *secretary, int n) {
    return list_get_nth_item(secretary->tasks, n);
}

Project *secretary_create_project(Secretary *secretary, const char* name) {
    Project *project = project_new(name);
    list_add_item(secretary->projects, project);
    return project;
}

int secretary_count_projects(Secretary *secretary) {
    return list_count_items(secretary->projects);
}

Project *secretary_get_project(Secretary *secretary, const char *name) {
    for (int i = 0; i < list_count_items(secretary->projects); i++) {
        Project *project = list_get_nth_item(secretary->projects, i);
        if (strcmp(name, project_get_name(project)) == 0) {
            return project;
        }
    }
    
    return NULL;
}

Project *secretary_get_nth_project(Secretary *secretary, int n) {
    return list_get_nth_item(secretary->projects, n);
}

void secretary_delete_task(Secretary *secretary, Task *task) {
    Project *project = task_get_project(task);
    if (project) {
        project_remove_task(project, task);
    }
    list_remove_item(secretary->tasks, task);
    task_free(task);
}

void secretary_delete_project(Secretary *secretary, Project *project) {
    list_remove_item(secretary->projects, project);
    project_free(project);
}

int secretary_count_inbox_tasks(Secretary *secretary, bool archived) {
    int counter = 0;
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        if (task_is_in_inbox(list_get_nth_item(secretary->tasks, i), archived))
            counter++;
    }
    return counter;
}

Task *secretary_get_nth_inbox_task(Secretary *secretary, int n, bool archived) {
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        if (task_is_in_inbox(list_get_nth_item(secretary->tasks, i), archived)) {
            if (n-- == 0) return list_get_nth_item(secretary->tasks, i);
        }
    }
    return NULL;
}

void secretary_archive_inbox_tasks(Secretary *secretary) {
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_done(task) && task_is_in_inbox(task, false)) {
            task_archive(task);
        }
    }
}

Task *secretary_get_task(Secretary *secretary, int number) {
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        if (task_get_number(list_get_nth_item(secretary->tasks, i)) == number) {
           return list_get_nth_item(secretary->tasks, i);
        }
    }
    return NULL;
}

void secretary_free(Secretary *secretary) {
    for (int i = 0; i < list_count_items(secretary->projects); i++) {
        project_free(list_get_nth_item(secretary->projects, i));
    }
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        task_free(list_get_nth_item(secretary->tasks, i));
    }
    free(secretary);
}

int secretary_count_tasks_scheduled(Secretary *secretary, bool archived) {
    int counter = 0;
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_scheduled(task) && task_is_archived(task) == archived) {
            counter++;
        }
    }
    return counter;
}


int secretary_count_tasks_scheduled_for(Secretary *secretary, struct tm date,
            bool archived) {
    int counter = 0;
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_scheduled_for(task, date) 
                && task_is_archived(task) == archived) {
           counter++;
        }
    }
    return counter;
}
int secretary_count_tasks_scheduled_for_today(Secretary *secretary, 
            bool archived) {
    time_t now = time(NULL);
    return secretary_count_tasks_scheduled_for(secretary, *localtime(&now), archived);
}

void secretary_archive_scheduled_tasks(Secretary *secretary) {
   for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_done(task) && task_is_scheduled(task)) {
            task_archive(task);
        }
    }
}
void secretary_archive_tasks_scheduled_for(Secretary *secretary, struct tm date) {
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_done(task) && 
                task_is_scheduled_for(task, date)) {
            task_archive(task);
        }
    }
}
void secretary_archive_tasks_scheduled_for_today(Secretary *secretary) {
    time_t now = time(NULL);
    secretary_archive_tasks_scheduled_for(secretary, *localtime(&now));
}

Task *secretary_get_nth_task_scheduled(Secretary *secretary, int n, 
            bool archived) {
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_scheduled(task) && task_is_archived(task) == archived) {
            if (n-- == 0) return list_get_nth_item(secretary->tasks, i);
        }
    }
    return NULL;
}

Task *secretary_get_nth_task_scheduled_for(Secretary *secretary, struct tm date, 
        int n, bool archived) {
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_scheduled_for(task, date) 
                && task_is_archived(task) == archived) {
            if (n-- == 0) return task;
        }
    }
    return NULL;
}
Task *secretary_get_nth_task_scheduled_for_today(Secretary *secretary, int n,
        bool archived) {
    time_t now = time(NULL);
    return 
        secretary_get_nth_task_scheduled_for(secretary, *localtime(&now), n, archived);
}


int secretary_count_done_tasks(Secretary *secretary, bool archived) {
    int counter = 0;
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_done(task) && task_is_archived(task) == archived) {
            counter++;
        }
    }
    return counter;
}

Task *secretary_get_nth_done_task(Secretary *secretary, int n, bool archived) {
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_done(task) && task_is_archived(task) == archived 
                && n-- == 0) {
            return task;
        }
    }
    return NULL;
}

/* INTERNAL INTERFACE: functions which should never be used by secretary clients
 */
void _secretary_register_in_inbox(Secretary *secretary, Task *task) {
    if (secretary) {
        list_add_item(secretary->inbox_perspective.visible_tasks, task);
    }
}

void _secretary_unregister_from_inbox(Secretary *secretary, Task *task) {
    if (secretary) {
        list_remove_item(secretary->inbox_perspective.visible_tasks, task);
    }
}

void _secretary_register_in_scheduled(Secretary *secretary, Task *task) {
    if (secretary) {
        list_add_item(secretary->scheduled_perspective.visible_tasks, task);
    }
}

void _secretary_unregister_from_scheduled(Secretary *secretary, Task *task) {
    if (secretary) {
        list_remove_item(secretary->scheduled_perspective.visible_tasks, task);
    }
}



