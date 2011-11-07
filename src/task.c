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
#include <secretary/task.h>
#include <secretary/project.h>
#include <secretary/util.h>
#include <stdlib.h>

#include <secretary/_internal/secretary.h>

#define TASK_SCHEDULED_VALUE_FOR_COMPARE(t) ((t)->scheduled_for*(t)->scheduled)

Task *task_new(const char *description) {
    Task *task = malloc(sizeof(Task));
    time(&task->created_at);
    task->description = util_copy_string(description);
    task->project = NULL;
    task->scheduled = false;
    task->archived = false;
    task->done = false;
    task->secretary = NULL;
    return task;
}

time_t task_get_creation_date(Task *task) {
    return task->created_at;
}

const char *task_get_description(Task *task) {
    return task->description;
}

void task_set_description(Task *task, const char *description) {
    free(task->description);
    task->description = util_copy_string(description);
}

Project *task_get_project(Task *task) {
    return task->project;
}

void task_set_project(Task *task, Project *project) {
    project_add_task(project, task);
}

bool task_has_project(Task *task) {
    return task->project != NULL;
}

bool task_is_in_project(Task *task, struct Project *project) {
    return task->project == project;
}

void task_unset_project(Task *task) {
    if (task->project == NULL) return;
    project_remove_task(task->project, task);
}

bool task_is_in_inbox(Task *task) {
    return task->project == NULL && !task->scheduled;
}

void task_schedule(Task *task, time_t date) {
    bool was_in_inbox = task_is_in_inbox(task),
        was_scheduled = task_is_scheduled(task);
    task->scheduled = true;
    task->scheduled_for = date-(date%(UTIL_SECONDS_IN_DAY));
    // For optimization of secretary
    if (was_in_inbox) {
        _secretary_unregister_from_inbox(task->secretary, task);
    }
    if (!was_scheduled) {
        _secretary_register_in_scheduled(task->secretary, task);
    } else {
        _secretary_sort_scheduled_tasks(task->secretary, task);
    }
}

time_t task_get_scheduled_date(Task *task) {
    return task->scheduled_for;
}

bool task_is_scheduled(Task *task) {
    return task->scheduled;
}

bool task_is_scheduled_for(Task *task, time_t date) {
    long scheduled_date = task->scheduled_for/UTIL_SECONDS_IN_DAY, // removing hours, minutes etc.
         compared_date = date/UTIL_SECONDS_IN_DAY;
    return task_is_scheduled(task) && scheduled_date <= compared_date;
}

void task_unschedule(Task *task) {
    task->scheduled = false;
    // For secretary optimization
    _secretary_unregister_from_scheduled(task->secretary, task);
    _secretary_register_in_inbox(task->secretary, task);
}

void task_mark_as_done(Task *task) {
    task->done = true;
}

void task_unmark_as_done(Task *task) {
    bool was_archived = task->archived;
    task->done = false;
    task->archived = false;
    // For secretary optimization
    if (was_archived) {
        _secretary_switch_list_in_inbox_perspective(task->secretary, task);
        _secretary_switch_list_in_scheduled_perspective(task->secretary, task);
    }
}

void task_switch_done_status(Task *task) {
    task->done = !task->done;
}

bool task_is_done(Task *task) {
    return task->done;
}

void task_archive(Task *task) {
    bool was_archived = task->archived;
    task->archived = task->done && true;
    if (!was_archived) {
        _secretary_switch_list_in_inbox_perspective(task->secretary, task);
        _secretary_switch_list_in_scheduled_perspective(task->secretary, task);
    }
}

bool task_is_archived(Task *task) {
    return task->archived;
}

void task_free(Task *task) {
    if (task_get_project(task) != NULL) {
        project_remove_task(task_get_project(task), task);
    }
    free(task->description);
    free(task);
}

int task_compare(Task *task1, Task *task2) {
    int result = task2->archived - task1->archived;
    if (result) return result;
    result = task1->scheduled - task2->scheduled;
    if (result) return result;
    if (task1->scheduled) result = task2->scheduled_for - task1->scheduled_for;
    if (result) return result;
    return task2->created_at - task1->created_at;
}

/* THE FOLLOWING FUNCTIONS SHOULD NOT BE USED BY EXTERNAL CLIENTS */

int _task_compare_by_date(const void *p1, const void* p2) {
    Task *task1 = *(Task**)p1, *task2 = *(Task**)p2;
    return task1->scheduled_for - task2->scheduled_for;
}
