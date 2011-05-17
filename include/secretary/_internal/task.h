/**
 * A "friend" interface for internal purproses. It should not be used by
 * libsecretary clients but only by libsecretary modules themselves.
 */
#ifndef _INTERNAL_TASK_H
#  define _INTERNAL_TASK_H

#include <secretary/task.h>


int _task_compare_by_date(const void *p1, const void* p2);

#endif

