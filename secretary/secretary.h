
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

typedef struct Secretary {
    int acc; // For strange purposes
    List *tasks, *projects;
    // Sublists of list of tasks, used for optimization
    List *visible_scheduled_tasks, *archived_scheduled_tasks,
         *visible_scheduled_for_today_tasks, *archived_scheduled_for_today_tasks,
         *visible_inbox,  *archived_inbox, *visible_tasks, *archived_tasks;
} Secretary;

Secretary *secretary_new();
Task *secretary_create_task(Secretary *secretary, const char* description);
int secretary_count_tasks(Secretary *secretary, bool archived);
int secretary_count_all_tasks(Secretary *secretary);
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
int secretary_count_tasks_scheduled_for(Secretary *secretary, time_t date, bool archived);
int secretary_count_tasks_scheduled_for_today(Secretary *secretary, bool archived);
Task *secretary_get_nth_task_scheduled(Secretary *secretary, int n, bool archived);
Task *secretary_get_nth_task_scheduled_for(Secretary *secretary, time_t date, int n, bool archived);
Task *secretary_get_nth_task_scheduled_for_today(Secretary *secretary, int n, bool archived);
void secretary_archive_scheduled_tasks(Secretary *secretary);
void secretary_archive_tasks_scheduled_for(Secretary *secretary, time_t date);
void secretary_archive_tasks_scheduled_for_today(Secretary *secretary);

int secretary_count_done_tasks(Secretary *secretary, bool archived);
Task *secretary_get_nth_done_task(Secretary *secretary, int n, bool archived);

void secretary_sort_tasks(Secretary *secretary);

void secretary_delete_task(Secretary *secretary, Task *task);
void secretary_delete_project(Secretary *secretary, Project *project);

void secretary_schedule_task(Secretary *secretary, Task *task, time_t time);
void secretary_unschedule_task(Secretary *secretary, Task *task);

void secretary_move_task_to_project(Secretary *secretary, Project *project, 
        Task *task);
void secretary_remove_task_from_project(Secretary *secretary, Task *task);

void secretary_archive_task(Secretary *secretary, Task *task);


void secretary_free(Secretary *secretary);

#endif