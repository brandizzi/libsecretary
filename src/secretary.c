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
    secretary->acc = 0;
    return secretary;
}

Task *secretary_create_task(Secretary *secretary, const char* description) {
    Task *task = task_new(description);
    task->secretary = secretary;
    task->number = ++secretary->acc;
    list_add_item(secretary->tasks, task);
#warning to optimize
    secretary_sort_tasks(secretary);
    return task;
}

int secretary_count_tasks(Secretary *secretary, bool archived) {
    void *params[] = { &archived };
    return list_count_items_by_criteria(
            secretary->tasks, _secretary_predicate_task_archival_is, params);
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
    void *params[] = { (void*) name };
    return list_get_nth_item_by_criteria(secretary->projects, 0,
            _secretary_predicate_project_is_named, params);
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
#warning to optimize
    secretary_sort_tasks(secretary);

    task_free(task);
}

void secretary_delete_project(Secretary *secretary, Project *project) {
    list_remove_item(secretary->projects, project);
    project_free(project);
}

int secretary_count_inbox_tasks(Secretary *secretary, bool archived) {
    void *params[] = { &archived };
    return list_count_items_by_criteria(secretary->tasks, 
            _secretary_predicate_task_is_in_inbox, params);
}

Task *secretary_get_nth_inbox_task(Secretary *secretary, int n, bool archived) {
    void *params[] = { &archived };
    return list_get_nth_item_by_criteria(secretary->tasks, n,
            _secretary_predicate_task_is_in_inbox, params);
}

void secretary_archive_inbox_tasks(Secretary *secretary) {
    bool archived = false;
    void *params[] = { &archived };
    int done_count = list_count_items_by_criteria(secretary->tasks,
                _secretary_predicate_inbox_task_is_done, params);
    for (int i = 0; i < done_count; i++) {
        Task *task = list_get_nth_item_by_criteria(secretary->tasks, 0,
                _secretary_predicate_inbox_task_is_done, params);
        task_archive(task);
    }
}

Task *secretary_get_task(Secretary *secretary, int number) {
#warning function secretary_get_task to be removed
    if (number < secretary_count_all_tasks(secretary)) {
        return list_get_nth_item(secretary->tasks, number);
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
    void *params[] = { &archived };
    return list_count_items_by_criteria(secretary->tasks, 
            _secretary_predicate_task_is_scheduled, params);
}


int secretary_count_tasks_scheduled_for(Secretary *secretary, time_t date,
            bool archived) {
    void *params[] = { &archived, &date };
    return list_count_items_by_criteria(secretary->tasks, 
            _secretary_predicate_task_is_scheduled_for, params);
}
int secretary_count_tasks_scheduled_for_today(Secretary *secretary, 
            bool archived) {
    return secretary_count_tasks_scheduled_for(secretary, time(NULL), archived);
}

void secretary_archive_scheduled_tasks(Secretary *secretary) {
    int scheduled_count = list_count_items_by_criteria(secretary->tasks, 
            _secretary_predicate_done_scheduled_task, NULL);
    for (int i = 0; i < scheduled_count; i++) {
        Task *task = list_get_nth_item_by_criteria(secretary->tasks, i,
                _secretary_predicate_done_scheduled_task, NULL);
        task_archive(task);
    }
}
void secretary_archive_tasks_scheduled_for(Secretary *secretary, time_t date) {
    void *params[] = { &date };
    int scheduled_count = list_count_items_by_criteria(secretary->tasks, 
            _secretary_predicate_done_task_scheduled_for, params);
    for (int i = 0; i < scheduled_count; i++) {
        Task *task = list_get_nth_item_by_criteria(secretary->tasks, i,
                _secretary_predicate_done_task_scheduled_for, params);
        task_archive(task);
    }
}
void secretary_archive_tasks_scheduled_for_today(Secretary *secretary) {
    secretary_archive_tasks_scheduled_for(secretary, time(NULL));
}

Task *secretary_get_nth_task_scheduled(Secretary *secretary, int n, 
            bool archived) {
    void *params[] = { &archived };
    return list_get_nth_item_by_criteria(secretary->tasks, n,
                _secretary_predicate_task_is_scheduled, params);
}

Task *secretary_get_nth_task_scheduled_for(Secretary *secretary, time_t date, 
        int n, bool archived) {
    void *params[] = { &archived, &date };
    return list_get_nth_item_by_criteria(secretary->tasks, n,
                _secretary_predicate_task_is_scheduled_for, params);
}
Task *secretary_get_nth_task_scheduled_for_today(Secretary *secretary, int n,
        bool archived) {
    return secretary_get_nth_task_scheduled_for(secretary, time(NULL), n, archived);
}

// WHY ON EARTH HAS IT AN archived ARGUMENT!?!?!?!
int secretary_count_done_tasks(Secretary *secretary, bool archived) {
    void *params[] = { &archived };
    return list_count_items_by_criteria(secretary->tasks,
            _secretary_predicate_task_is_done, params);
}

Task *secretary_get_nth_done_task(Secretary *secretary, int n, bool archived) {
    void *params[] = { &archived };
    return list_get_nth_item_by_criteria(secretary->tasks, n,
            _secretary_predicate_task_is_done, params);
}

void secretary_sort_tasks(Secretary *secretary) {
    list_sort(secretary->tasks, _secretary_task_compare);
}

/* INTERNAL INTERFACE: functions which should never be used by secretary clients
 */
/* UtilComparators */
int _secretary_task_compare(const void *p1, const void *p2) {
    const Task *task1 = *(Task**)p1, *task2 = *(Task**)p2;
    return task_compare(task1, task2);
}

/* Predicates */
bool _secretary_predicate_task_is_in_inbox(void *task, void **params) {
    bool archived = *(bool*)params[0];
    return task_is_in_inbox(task) && task_is_archived(task) == archived;
}

bool _secretary_predicate_task_archival_is(void *task, void **params) {
    bool archived = *(bool*)params[0];
    return task_is_archived(task) == archived;
}

bool _secretary_predicate_project_is_named(void *project, void **params) {
    const char *name = (const char*)params[0];
    return strcmp(project_get_name(project), name) == 0;
}

bool _secretary_predicate_task_is_done(void *task, void **params) {
    bool archived = params ? *(bool*)params[0] : false;
    return task_is_done(task) && task_is_archived(task) == archived;
}

bool _secretary_predicate_task_is_scheduled(void *task, void **params) {
    bool archived = *(bool*)params[0];
    return task_is_scheduled(task) && task_is_archived(task) == archived;
}

bool _secretary_predicate_task_is_scheduled_for(void *task, void **params) {
    bool archived = *(bool*)params[0];
    time_t date = *(time_t*)params[1];
    return task_is_scheduled_for(task, date) && task_is_archived(task) == archived;
}

bool _secretary_predicate_done_scheduled_task(void *task, void **params) {
    return task_is_done(task) && task_is_scheduled(task);
}

bool _secretary_predicate_done_task_scheduled_for(void *task, void **params) {
    time_t date = *(time_t*)params[0];
    return task_is_done(task) && task_is_scheduled_for(task, date);
}

bool _secretary_predicate_inbox_task_is_done(void *task, void **params) {
    bool archived = params ? *(bool*)params[0] : false;
    return task_is_in_inbox(task) && task_is_done(task) && task_is_archived(task) == archived;

}
 /*
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
*/
