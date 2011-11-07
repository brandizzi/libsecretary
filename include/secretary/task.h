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
#ifndef _TASK_H
# define _TASK_H

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

struct Project;
struct Secretary;

typedef struct {
    time_t created_at;
    char *description;
    struct Project *project;
    /**
     * This secretary reference is only useful for optimizations and should not
     * be used by any client. (Actually, no struct field should be directly
     * used by libsecretary clients at all.)
     */
    struct Secretary *secretary;

    bool scheduled;
    time_t scheduled_for;

    bool done;
    bool archived;
} Task;

Task *task_new(const char *description);

time_t task_get_creation_date(Task *task);
const char *task_get_description(Task *task);
void task_set_description(Task *task, const char *description);

struct Project *task_get_project(Task *task);
void task_set_project(Task *task, struct Project *project);
void task_unset_project(Task *task);
bool task_has_project(Task *task);
bool task_is_in_project(Task *task, struct Project *project);

bool task_is_in_inbox(Task *task);

void task_schedule(Task *task, time_t date);
time_t task_get_scheduled_date(Task *task);
void task_unschedule(Task *task);
bool task_is_scheduled(Task *task);
bool task_is_scheduled_for(Task *task, time_t date);

void task_mark_as_done(Task *task);
void task_unmark_as_done(Task *task);
void task_switch_done_status(Task *task);
bool task_is_done(Task *task);

void task_archive(Task *task);
bool task_is_archived(Task *task);

int task_compare(Task *task1, Task *task2);

void task_free(Task *task);

#endif
