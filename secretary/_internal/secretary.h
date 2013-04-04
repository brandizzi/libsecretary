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
/**
 * A "friend" interface for internal purproses. It should not be used by
 * libsecretary clients but only by libsecretary modules themselves.
 */
#ifndef _INTERNAL_SECRETARY_H
#  define _INTERNAL_SECRETARY_H

#include <secretary/secretary.h>
#include "secretary/pubsub.h"

int _secretary_task_compare(const void *p1, const void *p2);

void _secretary_sort_tasks(SctSecretary *secretary);
void _secretary_update_sublists(SctSecretary *secretary);

void _secretary_add_task(SctSecretary *secretary, SctTask *task);

bool _secretary_predicate_project_is_named(void *project, SctList *params);

bool _secretary_predicate_task_is_in_inbox(void *task, SctList *params);
bool _secretary_predicate_task_archival_is(void *task, SctList *params);
bool _secretary_predicate_task_is_done(void *task, SctList *params);

bool _secretary_predicate_task_is_scheduled(void *task, SctList *params);
bool _secretary_predicate_task_is_scheduled_for(void *task, SctList *params);
bool _secretary_predicate_done_scheduled_task(void *task, SctList *params);
bool _secretary_predicate_done_task_scheduled_for(void *task, SctList *params);
bool _secretary_predicate_task_is_not_scheduled_for_today(void *task, 
        SctList *params);
bool _secretary_predicate_inbox_task_is_done(void *task, SctList *params);

bool _secretary_project_is_not_archived(void *project, SctList *params);

void _secretary_on_task_change(const char *event_name, SctList *params);
void _secretary_sort(SctPublisher *publisher, const char *event_name, 
        SctList *params);

#endif
