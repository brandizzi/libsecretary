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
#ifndef _PROJECT_H
# define _PROJECT_H

#include <secretary/task.h>
#include <secretary/list.h>

#include <stdlib.h>

typedef struct Project {
    char *name;
    List *tasks;
    bool archived;
} Project;

Project *project_new(const char *name);
void project_add_task(Project *project, Task *task);
void project_remove_task(Project *project, Task *task);

const char* project_get_name (Project *project);
void project_set_name(Project *project, const char *name);

int project_count_tasks (Project *project, bool archived);
Task *project_get_task(Project *project, int number);
Task *project_get_nth_task(Project *project, int n, bool archived);

void project_archive_tasks(Project *project);

void project_archive(Project *project);
void project_unarchive(Project *project);
bool project_is_archived(Project *project);

void project_free(Project *project);

#endif
