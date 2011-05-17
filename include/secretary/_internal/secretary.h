/**
 * A "friend" interface for internal purproses. It should not be used by
 * libsecretary clients but only by libsecretary modules themselves.
 */
#ifndef _INTERNAL_SECRETARY_H
#  define _INTERNAL_SECRETARY_H

#include <secretary/secretary.h>


void _secretary_register_in_inbox(Secretary *secretary, Task *task);
void _secretary_unregister_from_inbox(Secretary *secretary, Task *task);

void _secretary_register_in_scheduled(Secretary *secretary, Task *task);
void _secretary_unregister_from_scheduled(Secretary *secretary, Task *task);

void _secretary_switch_list_in_inbox_perspective(Secretary *secretary, Task *task);
void _secretary_switch_list_in_scheduled_perspective(Secretary *secretary, Task *task);

List *_secretary_get_list_from_perspective(_SecretaryPerspective perspective,
        bool archived);

#endif
