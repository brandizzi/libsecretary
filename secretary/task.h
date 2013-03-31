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
#ifndef _SECRETARY_TASK_H
# define _SECRETARY_TASK_H

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include <secretary/pubsub.h>


struct SctProject;
//struct SctSecretary;

typedef struct {
    int number; // For internal purposes not really that important...
    time_t created_at;
    char *description;
    struct SctProject *project;

    bool scheduled;
    time_t scheduled_for;

    bool done;
    bool archived;
    SctPublisher *publisher;
} SctTask;

SctTask *sct_task_new(const char *description);

time_t sct_task_get_creation_date(SctTask *task);
const char *sct_task_get_description(SctTask *task);
void sct_task_set_description(SctTask *task, const char *description);

struct SctProject *sct_task_get_project(SctTask *task);
void sct_task_set_project(SctTask *task, struct SctProject *project);
void sct_task_unset_project(SctTask *task);
bool sct_task_has_project(SctTask *task);
bool sct_task_is_in_project(SctTask *task, struct SctProject *project);

bool sct_task_is_in_inbox(SctTask *task);

void sct_task_schedule(SctTask *task, time_t date);
time_t sct_task_get_scheduled_date(SctTask *task);
void sct_task_unschedule(SctTask *task);
bool sct_task_is_scheduled(SctTask *task);
bool sct_task_is_scheduled_for(SctTask *task, time_t date);

void sct_task_mark_as_done(SctTask *task);
void sct_task_unmark_as_done(SctTask *task);
void sct_task_switch_done_status(SctTask *task);
bool sct_task_is_done(SctTask *task);

void sct_task_archive(SctTask *task);
bool sct_task_is_archived(SctTask *task);

int sct_task_compare(const SctTask *task1, const SctTask *task2);

void sct_task_set_project_change_event_callback(SctTask *task, 
            SctPublisherCallback callback, void **params);

void sct_task_free(SctTask *task);

#endif
