
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


bool _secretary_predicate_task_is_in_inbox(void *task, void **params);

void _secretary_register_in_inbox(Secretary *secretary, Task *task);
void _secretary_unregister_from_inbox(Secretary *secretary, Task *task);

void _secretary_register_in_scheduled(Secretary *secretary, Task *task);
void _secretary_unregister_from_scheduled(Secretary *secretary, Task *task);

void _secretary_sort_scheduled_tasks(Secretary *secretary, Task *task);

void _secretary_switch_list_in_inbox_perspective(Secretary *secretary, Task *task);
void _secretary_switch_list_in_scheduled_perspective(Secretary *secretary, Task *task);

List *_secretary_get_list_from_perspective(_SecretaryPerspective perspective,
        bool archived);

#endif
