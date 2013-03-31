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
#include <secretary/project.h>
#include <secretary/util.h>
#include <secretary/_internal/secretary.h>
#include <secretary/_internal/project.h>
#include <stdlib.h>
#include <string.h>

#define SCT_PROJECT_CHANGE_EVENT "sct-project-change"

SctProject *sct_project_new(const char *name) {
    SctProject *project = malloc(sizeof(SctProject));

    project->name = sct_util_copy_string(name);
    project->tasks = sct_list_new();
    project->archived = false;
    project->publisher = sct_publisher_new();

    return project;
}

void sct_project_add_task(SctProject *project, SctTask *task) {
    sct_task_unset_project(task);
    sct_list_add_item(project->tasks, task);
    task->project = project;
    _project_sort_tasks(project);
    
    sct_publisher_trigger(project->publisher, SCT_PROJECT_CHANGE_EVENT);
}

SctTask *sct_project_get_task(SctProject *project, int index) {
    if (index < sct_list_count_items(project->tasks)) 
        return sct_list_get_nth_item(project->tasks, index);
    return NULL;
}

SctTask *sct_project_get_nth_task(SctProject *project, int n, bool archived) {
    for (int i = 0; i < sct_list_count_items(project->tasks); i++) {
        SctTask *task = sct_list_get_nth_item(project->tasks, i);
        if (sct_task_is_archived(task) == archived && n-- == 0) {
            return task;
        }
    }
    return NULL;
}

int sct_project_count_tasks(SctProject *project, bool archived) {
    int counter = 0;
    for (int i = 0; i < sct_list_count_items(project->tasks); i++) {
        SctTask *task = sct_list_get_nth_item(project->tasks, i);
        if (sct_task_is_archived(task) == archived) {
            counter++;
        }
    }
    return counter;
}

void sct_project_remove_task(SctProject *project, SctTask *task) {
    sct_list_remove_item(project->tasks, task);
    task->project = NULL;
    _project_sort_tasks(project);
    
    sct_publisher_trigger(project->publisher, SCT_PROJECT_CHANGE_EVENT);
}

const char* sct_project_get_name(SctProject *project) {
    return project->name;
}
void sct_project_set_name(SctProject *project, const char *name) {
    free(project->name);
    project->name = sct_util_copy_string(name);
    
    sct_publisher_trigger(project->publisher, SCT_PROJECT_CHANGE_EVENT);
}

void sct_project_archive_tasks(SctProject *project) {
   for (int i = 0; i < sct_list_count_items(project->tasks); i++) {
        SctTask *task = sct_list_get_nth_item(project->tasks, i);
        if (sct_task_is_done(task)) {
            sct_task_archive(task);
        }
    }
    _project_sort_tasks(project);
    
    sct_publisher_trigger(project->publisher, SCT_PROJECT_CHANGE_EVENT);
}

void sct_project_free(SctProject *project) {
    for (int i = 0; i < sct_list_count_items(project->tasks); i++) {
        SctTask *task = sct_list_get_nth_item(project->tasks, i);
        task->project = NULL;
    }
    sct_publisher_free(project->publisher);
    free(project->name);
    free(project);
}


void sct_project_archive(SctProject *project) {
    project->archived = true;
    
    sct_publisher_trigger(project->publisher, SCT_PROJECT_CHANGE_EVENT);
}

void sct_project_unarchive(SctProject *project) {
    project->archived = false;
    
    sct_publisher_trigger(project->publisher, SCT_PROJECT_CHANGE_EVENT);
}

bool sct_project_is_archived(SctProject *project) {
    return project->archived;
}

void sct_project_set_change_event_callback(SctProject *project, 
        SctPublisherCallback callback, void **params) {
    sct_publisher_add_event(project->publisher, SCT_PROJECT_CHANGE_EVENT,
        callback, params);    
}

/** INTERNAL FUNCTIONS */
void _project_sort_tasks(SctProject *project) {
    sct_list_sort(project->tasks, _secretary_task_compare);
}
