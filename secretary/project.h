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
#ifndef _SECRETARY_PROJECT_H
# define _SECRETARY_PROJECT_H

#include "secretary/task.h"
#include "secretary/list.h"
#include "secretary/pubsub.h"

#include <stdlib.h>

typedef struct SctProject {
    char *name;
    SctList *tasks;
    bool archived;
    SctPublisher *publisher;
} SctProject;

SctProject *sct_project_new(const char *name);
void sct_project_add_task(SctProject *project, SctTask *task);
void sct_project_remove_task(SctProject *project, SctTask *task);

const char* sct_project_get_name (SctProject *project);
void sct_project_set_name(SctProject *project, const char *name);

int sct_project_count_tasks (SctProject *project, bool archived);
SctTask *sct_project_get_task(SctProject *project, int number);
SctTask *sct_project_get_nth_task(SctProject *project, int n, bool archived);

void sct_project_archive_tasks(SctProject *project);

void sct_project_archive(SctProject *project);
void sct_project_unarchive(SctProject *project);
bool sct_project_is_archived(SctProject *project);

void sct_project_free(SctProject *project);

#endif
