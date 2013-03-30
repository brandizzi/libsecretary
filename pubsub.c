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
#include <secretary/pubsub.h>

#include <string.h>

typedef struct SctPublisherEvent {
    const char *event_name;
    SctPublisherCallback callback;
    void **params;
} SctPublisherEvent;

SctPublisher *sct_publisher_new() {
    SctPublisher *publisher = malloc(sizeof(SctPublisher));
    publisher->events = sct_list_new();
    return publisher;
}

static bool has_event_name(void *item, void **params);

void sct_publisher_add_event(SctPublisher *publisher, const char *event_name, 
        SctPublisherCallback callback, void **params) {
    SctPublisherEvent *event = malloc(sizeof(SctPublisherEvent));
    event->event_name = event_name;
    event->callback = callback;
    event->params = params;
    sct_list_add_item(publisher->events, event);
}
        
void sct_publisher_trigger(SctPublisher *publisher, const char *event_name) {
    void *names[] = { event_name };
    SctPublisherEvent *event =  sct_list_get_nth_item_by_criteria(
        publisher->events, 0, has_event_name, names);
    if (event) {
        SctPublisherCallback callback = event->callback;
        callback(event->event_name, event->params);
    }
    
}
void sct_publisher_free(SctPublisher *publisher) {
    SctList *events = publisher->events;
    for (int i = 0; i < sct_list_count_items(events); i++) {
        SctPublisherEvent *event = sct_list_get_nth_item(events, i);
        free(event);
    }
    sct_list_free(events);
    free(publisher);
}

static bool has_event_name(void *item, void **params) {
    SctPublisherEvent *event = item;
    const char *event_name = params[0];
    return strcmp(event->event_name, event_name) == 0;
}

