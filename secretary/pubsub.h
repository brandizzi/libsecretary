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
 * An object which stores callbacks and execute them with required.
 */
#ifndef _SECRETARY_PUBSUB_H
# define _SECRETARY_PUBSUB_H

typedef void (*SctPublisherCallback)(const char *event_name, void *params);

typedef struct SctPublisher {
} SctPublisher;


SctPublisher *sct_publisher_new();
void sct_publisher_add_event(SctPublisher *publisher, const char *event_name, 
        SctPublisherCallback callback, void *params);
void sct_publisher_trigger(SctPublisher *publisher, const char *event_name);
void sct_publisher_free(SctPublisher *publisher);

#endif
