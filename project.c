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

Project *project_new(const char *name) {
    Project *project = malloc(sizeof(Project));
    project->name = util_copy_string(name);
    project->tasks = list_new();
    project->archived = false;
    return project;
}

void project_add_task(Project *project, Task *task) {
    task_unset_project(task);
    list_add_item(project->tasks, task);
    task->project = project;
    _project_sort_tasks(project);
}

Task *project_get_task(Project *project, int index) {
    if (index < list_count_items(project->tasks)) 
        return list_get_nth_item(project->tasks, index);
    return NULL;
}

Task *project_get_nth_task(Project *project, int n, bool archived) {
    for (int i = 0; i < list_count_items(project->tasks); i++) {
        Task *task = list_get_nth_item(project->tasks, i);
        if (task_is_archived(task) == archived && n-- == 0) {
            return task;
        }
    }
    return NULL;
}

int project_count_tasks(Project *project, bool archived) {
    int counter = 0;
    for (int i = 0; i < list_count_items(project->tasks); i++) {
        Task *task = list_get_nth_item(project->tasks, i);
        if (task_is_archived(task) == archived) {
            counter++;
        }
    }
    return counter;
}

void project_remove_task(Project *project, Task *task) {
    list_remove_item(project->tasks, task);
    task->project = NULL;
    _project_sort_tasks(project);
}

const char* project_get_name(Project *project) {
    return project->name;
}
void project_set_name(Project *project, const char *name) {
    free(project->name);
    project->name = util_copy_string(name);
}

void project_archive_tasks(Project *project) {
   for (int i = 0; i < list_count_items(project->tasks); i++) {
        Task *task = list_get_nth_item(project->tasks, i);
        if (task_is_done(task)) {
            task_archive(task);
        }
    }
    _project_sort_tasks(project);
}

void project_free(Project *project) {
    for (int i = 0; i < list_count_items(project->tasks); i++) {
        Task *task = list_get_nth_item(project->tasks, i);
        task->project = NULL;
    }
    free(project->name);
    free(project);
}


void project_archive(Project *project) {
    project->archived = true;
}

void project_unarchive(Project *project) {
    project->archived = false;
}

bool project_is_archived(Project *project) {
    return project->archived;
}

/** INTERNAL FUNCTIONS */
void _project_sort_tasks(Project *project) {
    list_sort(project->tasks, _secretary_task_compare);
}
