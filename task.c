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
#include <string.h>

#define TASK_SCHEDULED_VALUE_FOR_COMPARE(t) ((t)->scheduled_for*(t)->scheduled)

#define SCT_TASK_PROJECT_CHANGE_EVENT "task-project-change"

SctTask *sct_task_new(const char *description) {
    SctTask *task = malloc(sizeof(SctTask));
    time(&task->created_at);
    task->description = sct_util_copy_string(description);
    task->project = NULL;
    task->scheduled = false;
    task->archived = false;
    task->done = false;
    task->number = 0;
    task->publisher = sct_publisher_new();
    return task;
}

time_t sct_task_get_creation_date(SctTask *task) {
    return task->created_at;
}

const char *sct_task_get_description(SctTask *task) {
    return task->description;
}

void sct_task_set_description(SctTask *task, const char *description) {
    free(task->description);
    task->description = sct_util_copy_string(description);
}

SctProject *sct_task_get_project(SctTask *task) {
    return task->project;
}

void sct_task_set_project(SctTask *task, SctProject *project) {
    sct_project_add_task(project, task);
    sct_publisher_trigger(task->publisher, SCT_TASK_PROJECT_CHANGE_EVENT);
}

bool sct_task_has_project(SctTask *task) {
    return task->project != NULL;
}

bool sct_task_is_in_project(SctTask *task, struct SctProject *project) {
    return task->project == project;
}

void sct_task_unset_project(SctTask *task) {
    if (task->project == NULL) return;
    sct_project_remove_task(task->project, task);
}

bool sct_task_is_in_inbox(SctTask *task) {
    return task->project == NULL && !task->scheduled;
}

void sct_task_schedule(SctTask *task, time_t date) {
    task->scheduled = true;
    task->scheduled_for = sct_util_beginning_of_day(date);
}

time_t sct_task_get_scheduled_date(SctTask *task) {
    return task->scheduled_for;
}

bool sct_task_is_scheduled(SctTask *task) {
    return task->scheduled;
}

bool sct_task_is_scheduled_for(SctTask *task, time_t date) {
    return task->scheduled && task->scheduled_for <= sct_util_beginning_of_day(date);
}

void sct_task_unschedule(SctTask *task) {
    task->scheduled = false;
}

void sct_task_mark_as_done(SctTask *task) {
    task->done = true;
}

void sct_task_unmark_as_done(SctTask *task) {
    task->done = false;
    task->archived = false;
}

void sct_task_switch_done_status(SctTask *task) {
    task->done = !task->done;
}

bool sct_task_is_done(SctTask *task) {
    return task->done;
}

void sct_task_archive(SctTask *task) {
    task->archived = task->done && true;
}

bool sct_task_is_archived(SctTask *task) {
    SctProject *project = task->project;
    return task->archived || (project != NULL && sct_project_is_archived(project));
}

void sct_task_free(SctTask *task) {
    if (sct_task_get_project(task) != NULL) {
        sct_project_remove_task(sct_task_get_project(task), task);
    }
    sct_publisher_free(task->publisher);
    free(task->description);
    free(task);
}

void sct_task_set_project_change_event_callback(SctTask *task, 
            SctPublisherCallback callback, void **params) {
    sct_publisher_add_event(task->publisher, SCT_TASK_PROJECT_CHANGE_EVENT, 
            callback, params);
}

int sct_task_compare(const SctTask *task1, const SctTask *task2) {
    int result;
    // Archived are greater than non archived
    result = task1->archived - task2->archived;
    if (result) return result;
    // Scheduled are greater than non scheduled`
    result = task2->scheduled - task1->scheduled;
    if (result) return result;
    // Scheduled for later are greater than scheduled for sooner
    if (task1->scheduled) {
        result = task1->scheduled_for - task2->scheduled_for;
    } else {
        // If not scheduled, tasks in project are smaller than non in project
        result = (task2->project != NULL) - (task1->project != NULL);
    }
    if (result) return result;
    // Created later are greater than created sooner
    result = task1->created_at - task2->created_at;
    if (result) return result;
    // High numbered are greater than low numbered
    result = task1->number - task2->number;
    if (result) return result;
    // Ordered by name
    return strcmp(task1->description, task2->description);
}


