/**
 * libsecretary: a C library for managing to-do lists
 * Copyright (C) 2011  Adam Victor Nazareth Brandizzi <brandizzi@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * You can get the latest version of this file at 
 * http://bitbucket.org/brandizzi/libsecretary/
 */
#include <secretary/secretary.h>
#include <secretary/_internal/secretary.h>
#include <secretary/_internal/task.h>

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

int secretary_count_tasks(Secretary *secretary, bool archived) {
#warning to optimize (maybe using perspectives)
    int counter = 0;
    for (int i = 0; i < list_count_items(secretary->tasks); i++) {
        Task *task = list_get_nth_item(secretary->tasks, i);
        if (task_is_archived(task) == archived) { 
            counter++;
        }
    }
    return counter;
}

int secretary_count_all_tasks(Secretary *secretary) {
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

    _secretary_unregister_from_inbox(secretary, task);
    _secretary_unregister_from_scheduled(secretary, task);

    task_free(task);
}

void secretary_delete_project(Secretary *secretary, Project *project) {
    list_remove_item(secretary->projects, project);
    project_free(project);
}

int secretary_count_inbox_tasks(Secretary *secretary, bool archived) {
    List *list = _secretary_get_list_from_perspective(
            secretary->inbox_perspective, archived);
    return list_count_items(list);
}

Task *secretary_get_nth_inbox_task(Secretary *secretary, int n, bool archived) {
    List *list = _secretary_get_list_from_perspective(
            secretary->inbox_perspective, archived);
    return list_get_nth_item(list, n);
}

void secretary_archive_inbox_tasks(Secretary *secretary) {
    List *list = _secretary_get_list_from_perspective(
            secretary->inbox_perspective, false);
    for (int i = 0; i < list_count_items(list); i++) {
        Task *task = list_get_nth_item(list, i);
        if (task_is_done(task)) {
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
    list_free(secretary->inbox_perspective.archived_tasks);
    list_free(secretary->inbox_perspective.visible_tasks);
    list_free(secretary->scheduled_perspective.archived_tasks);
    list_free(secretary->scheduled_perspective.visible_tasks);
    free(secretary);
}

int secretary_count_tasks_scheduled(Secretary *secretary, bool archived) {
    List *list = _secretary_get_list_from_perspective(
            secretary->scheduled_perspective, archived);
    return list_count_items(list);
}


int secretary_count_tasks_scheduled_for(Secretary *secretary, struct tm date,
            bool archived) {
    // Note that the function assumes an ordered list!
    int counter = 0;
    List *list = _secretary_get_list_from_perspective(
            secretary->scheduled_perspective, archived);
    for (counter = 0; counter < list_count_items(list); counter++) {
        Task *task = list_get_nth_item(list, counter);
        #warning inefficient creation of date
        if (!task_is_scheduled_for(task, mktime(&date))) break;
    }
    return counter;
}
int secretary_count_tasks_scheduled_for_today(Secretary *secretary, 
            bool archived) {
    time_t now = time(NULL);
    return secretary_count_tasks_scheduled_for(secretary, *localtime(&now), archived);
}

void secretary_archive_scheduled_tasks(Secretary *secretary) {
    List *list = _secretary_get_list_from_perspective(
            secretary->scheduled_perspective, false);
   for (int i = 0; i < list_count_items(list); i++) {
        Task *task = list_get_nth_item(list, i);
        if (task_is_done(task)) {
            task_archive(task);
        }
    }
}
void secretary_archive_tasks_scheduled_for(Secretary *secretary, struct tm date) {
    List *list = _secretary_get_list_from_perspective(
            secretary->scheduled_perspective, false);
    for (int i = 0; i < list_count_items(list); i++) {
        Task *task = list_get_nth_item(list, i);
        #warning inefficient creation of dates
        if (task_is_done(task) && 
                task_is_scheduled_for(task, mktime(&date))) {
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
    List *list = _secretary_get_list_from_perspective(
            secretary->scheduled_perspective, archived);
    return list_get_nth_item(list, n);
}

Task *secretary_get_nth_task_scheduled_for(Secretary *secretary, struct tm date, 
        int n, bool archived) {
    List *list = _secretary_get_list_from_perspective(
            secretary->scheduled_perspective, archived);
    Task *task = list_get_nth_item(list, n);
    // Making use of the ordered list
    if (task && task_is_scheduled_for(task, mktime(&date))) {
        return task;
    } else {
        return NULL;
    }
}
Task *secretary_get_nth_task_scheduled_for_today(Secretary *secretary, int n,
        bool archived) {
    time_t now = time(NULL);
    return secretary_get_nth_task_scheduled_for(secretary, *localtime(&now), n, archived);
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
        if (task_is_in_inbox(task)) {
            List *list = _secretary_get_list_from_perspective(
                secretary->inbox_perspective, task->archived);
            list_add_item(list, task);
        }
    }
}

void _secretary_unregister_from_inbox(Secretary *secretary, Task *task) {
    if (secretary) {
        List *list = _secretary_get_list_from_perspective(
                secretary->inbox_perspective, task->archived);
        list_remove_item(list, task);
    }
}

void _secretary_register_in_scheduled(Secretary *secretary, Task *task) {
    if (secretary && task_is_scheduled(task)) {
        List *list = _secretary_get_list_from_perspective(
                secretary->scheduled_perspective, task->archived);
        list_add_item(list, task);
        // The schedule lists should always be sorted
        _secretary_sort_scheduled_tasks(secretary, task);
    }
}

void _secretary_sort_scheduled_tasks(Secretary *secretary, Task *task) {
    if (secretary && task_is_scheduled(task)) {
        List *list = _secretary_get_list_from_perspective(
                secretary->scheduled_perspective, task->archived);
        // The schedule lists should always be sorted
        list_sort(list, _task_compare_by_date);
    }
}


void _secretary_unregister_from_scheduled(Secretary *secretary, Task *task) {
    if (secretary) {
        List *list = _secretary_get_list_from_perspective(
                secretary->scheduled_perspective, task->archived);
        list_remove_item(list, task);
    }
}

void _secretary_switch_list_in_inbox_perspective(Secretary *secretary, Task *task) {
    if (secretary && task_is_in_inbox(task)) {
        List *origin = _secretary_get_list_from_perspective(
                secretary->inbox_perspective, !task->archived),
             *destination = _secretary_get_list_from_perspective(
                secretary->inbox_perspective, task->archived);
        list_remove_item(origin, task);
        list_add_item(destination, task);
    }
}

void _secretary_switch_list_in_scheduled_perspective(Secretary *secretary, Task *task) {
    if (secretary && task_is_scheduled(task)) {
        List *origin = _secretary_get_list_from_perspective(
                secretary->scheduled_perspective, !task->archived),
             *destination = _secretary_get_list_from_perspective(
                secretary->scheduled_perspective, task->archived);
        list_remove_item(origin, task);
        list_add_item(destination, task);
        // The schedule lists should always be sorted
        list_sort(destination, _task_compare_by_date);

    }
}

List *_secretary_get_list_from_perspective(_SecretaryPerspective perspective,
        bool archived) {
    if (archived) {
        return perspective.archived_tasks;
    } else {
        return perspective.visible_tasks;
    }
}

