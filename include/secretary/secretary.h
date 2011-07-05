
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
#ifndef _SECRETARY_H
# define _SECRETARY_H

#include <secretary/task.h>
#include <secretary/project.h>
#include <secretary/list.h>
#include <time.h>

/**
 * This is a private data type that should not be used by secretary clients
 */ 
typedef struct _SecretaryPerspective {
    List *archived_tasks, *visible_tasks;
} _SecretaryPerspective;

typedef struct Secretary {
    List *tasks, *projects;
    /**
     * This struct represents some perspectives of the secretary. It allows
     * secretary to somewhat "cache" inbox and scheduled tasks. 
     */
    struct _SecretaryPerspective inbox_perspective, scheduled_perspective;
} Secretary;

Secretary *secretary_new();
Task *secretary_create_task(Secretary *secretary, const char* description);
int secretary_count_tasks(Secretary *secretary);
Task *secretary_get_task(Secretary *secretary, int number);
Task *secretary_get_nth_task(Secretary *secretary, int n);

int secretary_count_inbox_tasks(Secretary *secretary, bool archived);
Task *secretary_get_nth_inbox_task(Secretary *secretary, int n, bool archived);
void secretary_archive_inbox_tasks(Secretary *secretary);

Project *secretary_create_project(Secretary *secretary, const char* name);
int secretary_count_projects(Secretary *secretary);
Project *secretary_get_project(Secretary *secretary, const char *name);
Project *secretary_get_nth_project(Secretary *secretary, int n);

int secretary_count_tasks_scheduled(Secretary *secretary, bool archived);
int secretary_count_tasks_scheduled_for(Secretary *secretary, struct tm date, bool archived);
int secretary_count_tasks_scheduled_for_today(Secretary *secretary, bool archived);
Task *secretary_get_nth_task_scheduled(Secretary *secretary, int n, bool archived);
Task *secretary_get_nth_task_scheduled_for(Secretary *secretary, struct tm date, int n, bool archived);
Task *secretary_get_nth_task_scheduled_for_today(Secretary *secretary, int n, bool archived);
void secretary_archive_scheduled_tasks(Secretary *secretary);
void secretary_archive_tasks_scheduled_for(Secretary *secretary, struct tm date);
void secretary_archive_tasks_scheduled_for_today(Secretary *secretary);

int secretary_count_done_tasks(Secretary *secretary, bool archived);
Task *secretary_get_nth_done_task(Secretary *secretary, int n, bool archived);

void secretary_delete_task(Secretary *secretary, Task *task);
void secretary_delete_project(Secretary *secretary, Project *project);

void secretary_free(Secretary *secretary);

#endif
