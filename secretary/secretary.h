
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
#ifndef _SECRETARY_SECRETARY_H
# define _SECRETARY_SECRETARY_H

#include <secretary/task.h>
#include <secretary/project.h>
#include <secretary/list.h>
#include <time.h>

typedef struct SctSecretary {
    int acc; // For strange purposes
    SctList *tasks, *projects;
    // Sublists of list of tasks, used for optimization
    SctList *visible_scheduled_tasks, *archived_scheduled_tasks,
         *visible_scheduled_for_today_tasks, *archived_scheduled_for_today_tasks,
         *visible_inbox,  *archived_inbox, *visible_tasks, *archived_tasks;
} SctSecretary;

SctSecretary *sct_secretary_new();
SctTask *sct_secretary_create_task(SctSecretary *secretary, const char* description);
int sct_secretary_count_tasks(SctSecretary *secretary, bool archived);
int sct_secretary_count_all_tasks(SctSecretary *secretary);
SctTask *sct_secretary_get_task(SctSecretary *secretary, int number);
SctTask *sct_secretary_get_nth_task(SctSecretary *secretary, int n);

int sct_secretary_count_inbox_tasks(SctSecretary *secretary, bool archived);
SctTask *sct_secretary_get_nth_inbox_task(SctSecretary *secretary, int n, bool archived);
void sct_secretary_archive_inbox_tasks(SctSecretary *secretary);

SctProject *sct_secretary_create_project(SctSecretary *secretary, const char* name);
int sct_secretary_count_projects(SctSecretary *secretary);
SctProject *sct_secretary_get_project(SctSecretary *secretary, const char *name);
SctProject *sct_secretary_get_nth_project(SctSecretary *secretary, int n);
void sct_secretary_archive_project(SctSecretary *secretary, SctProject *project);


int sct_secretary_count_tasks_scheduled(SctSecretary *secretary, bool archived);
int sct_secretary_count_tasks_scheduled_for(SctSecretary *secretary, time_t date, bool archived);
int sct_secretary_count_tasks_scheduled_for_today(SctSecretary *secretary, bool archived);
SctTask *sct_secretary_get_nth_task_scheduled(SctSecretary *secretary, int n, bool archived);
SctTask *sct_secretary_get_nth_task_scheduled_for(SctSecretary *secretary, time_t date, int n, bool archived);
SctTask *sct_secretary_get_nth_task_scheduled_for_today(SctSecretary *secretary, int n, bool archived);
void sct_secretary_archive_scheduled_tasks(SctSecretary *secretary);
void sct_secretary_archive_tasks_scheduled_for(SctSecretary *secretary, time_t date);
void sct_secretary_archive_tasks_scheduled_for_today(SctSecretary *secretary);

int sct_secretary_count_done_tasks(SctSecretary *secretary, bool archived);
SctTask *sct_secretary_get_nth_done_task(SctSecretary *secretary, int n, bool archived);

void sct_secretary_sort_tasks(SctSecretary *secretary);

void sct_secretary_delete_task(SctSecretary *secretary, SctTask *task);
void sct_secretary_delete_project(SctSecretary *secretary, SctProject *project);

void sct_secretary_schedule_task(SctSecretary *secretary, SctTask *task, time_t time);
void sct_secretary_unschedule_task(SctSecretary *secretary, SctTask *task);

void sct_secretary_move_task_to_project(SctSecretary *secretary, SctProject *project, 
        SctTask *task);
void sct_secretary_remove_task_from_project(SctSecretary *secretary, SctTask *task);
void sct_secretary_archive_tasks_from_project(SctSecretary *secretary, SctProject *project);

void sct_secretary_archive_task(SctSecretary *secretary, SctTask *task);


void sct_secretary_free(SctSecretary *secretary);

#endif
