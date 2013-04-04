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
#include <secretary/secretary.h>
#include <secretary/_internal/secretary.h>
#include <secretary/_internal/task.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

SctSecretary *sct_secretary_new() {
    SctSecretary *secretary = calloc(1, sizeof(SctSecretary));
    secretary->tasks = sct_list_new();
    secretary->projects = sct_list_new();
    secretary->acc = 0;

    /* Optimization sublists */
    // Visible and archived
    secretary->visible_tasks = sct_sublist_new(secretary->tasks, 0, 0);
    secretary->archived_tasks = sct_sublist_new(secretary->tasks, 0, 0);
    // Scheduled tasks lists
    secretary->visible_scheduled_tasks = sct_sublist_new(secretary->tasks, 0, 0);
    secretary->archived_scheduled_tasks = sct_sublist_new(secretary->tasks, 0, 0);
    // Scheduled for today tasks lists
    secretary->visible_scheduled_for_today_tasks = 
            sct_sublist_new(secretary->tasks, 0, 0);
    secretary->archived_scheduled_for_today_tasks = 
            sct_sublist_new(secretary->tasks, 0, 0);
    // Inbox
    secretary->visible_inbox = sct_sublist_new(secretary->tasks, 0, 0);
    secretary->archived_inbox = sct_sublist_new(secretary->tasks, 0, 0);

    return secretary;
}

SctTask *sct_secretary_create_task(SctSecretary *secretary, const char* description) {
    SctTask *task = sct_task_new(description);

    task->number = ++secretary->acc;
    sct_list_add_item(secretary->tasks, task);
    
    _secretary_update_sublists(secretary);

    return task;
}

int sct_secretary_count_tasks(SctSecretary *secretary, bool archived) {
    SctList *list = archived ? 
            secretary->archived_tasks : 
            secretary->visible_tasks;
    return sct_list_count_items(list);
}

int sct_secretary_count_all_tasks(SctSecretary *secretary) {
    return sct_list_count_items(secretary->tasks);
}

SctTask *sct_secretary_get_nth_task(SctSecretary *secretary, int n) {
    return sct_list_get_nth_item(secretary->tasks, n);
}

SctProject *sct_secretary_create_project(SctSecretary *secretary, const char* name) {
    SctProject *project = sct_project_new(name);
    
    sct_list_add_item(secretary->projects, project);
   SctList *params = sct_list_new_with(1, secretary);;
    sct_project_set_change_event_callback(project, 
            _secretary_sort, params);
    
    return project;
}

int sct_secretary_count_projects(SctSecretary *secretary) {
    return sct_list_count_items_by_criteria(secretary->projects, 
            _secretary_project_is_not_archived, NULL);
}

SctProject *sct_secretary_get_project(SctSecretary *secretary, const char *name) {
    SctList *params = sct_list_new_with(1, (void*)name);
    return sct_list_get_nth_item_by_criteria(secretary->projects, 0,
            _secretary_predicate_project_is_named, params);
}

SctProject *sct_secretary_get_nth_project(SctSecretary *secretary, int n) {
    SctProject *p = sct_list_get_nth_item_by_criteria(secretary->projects, n,
            _secretary_project_is_not_archived, NULL);
}

void sct_secretary_archive_project(SctSecretary *secretary, SctProject *project) {
    sct_project_archive(project);
    _secretary_update_sublists(secretary);
}


void sct_secretary_delete_task(SctSecretary *secretary, SctTask *task) {
    SctProject *project = sct_task_get_project(task);
    if (project) {
        sct_project_remove_task(project, task);
    }
    sct_list_remove_item(secretary->tasks, task);
    _secretary_update_sublists(secretary);
    sct_task_free(task);
}

void sct_secretary_delete_project(SctSecretary *secretary, SctProject *project) {
    sct_list_remove_item(secretary->projects, project);
    sct_project_free(project);
    _secretary_update_sublists(secretary);    
}

int sct_secretary_count_inbox_tasks(SctSecretary *secretary, bool archived) {
    SctList *list = archived ? 
            secretary->archived_inbox : secretary->visible_inbox;
    return  sct_list_count_items(list);
}

SctTask *sct_secretary_get_nth_inbox_task(SctSecretary *secretary, int n, bool archived) {
    SctList *list = archived ? 
            secretary->archived_inbox : secretary->visible_inbox;
    return sct_list_get_nth_item(list, n);
}

void sct_secretary_archive_inbox_tasks(SctSecretary *secretary) {
    bool archived = false;
    SctList *params = sct_list_new_with(1, &archived);
    int done_count = sct_list_count_items_by_criteria(secretary->tasks,
                _secretary_predicate_inbox_task_is_done, params);
    for (int i = 0; i < done_count; i++) {
        SctTask *task = sct_list_get_nth_item_by_criteria(secretary->tasks, 0,
                _secretary_predicate_inbox_task_is_done, params);
        sct_task_archive(task);
    }
    _secretary_update_sublists(secretary);
}

SctTask *sct_secretary_get_task(SctSecretary *secretary, int number) {
#warning function sct_secretary_get_task to be removed
    if (number < sct_secretary_count_all_tasks(secretary)) {
        return sct_list_get_nth_item(secretary->tasks, number);
    }
    return NULL;
}

void sct_secretary_free(SctSecretary *secretary) {
    for (int i = 0; i < sct_list_count_items(secretary->projects); i++) {
        sct_project_free(sct_list_get_nth_item(secretary->projects, i));
    }
    for (int i = 0; i < sct_list_count_items(secretary->tasks); i++) {
        sct_task_free(sct_list_get_nth_item(secretary->tasks, i));
    }
    free(secretary);
}

int sct_secretary_count_tasks_scheduled(SctSecretary *secretary, bool archived) {
    SctList *list = archived ? 
            secretary->archived_scheduled_tasks : 
            secretary->visible_scheduled_tasks;
    return sct_list_count_items(list);
}


int sct_secretary_count_tasks_scheduled_for(SctSecretary *secretary, time_t date,
            bool archived) {
    SctList *params = sct_list_new_with(2, &archived, &date);
    SctList *list = archived ? 
            secretary->archived_scheduled_tasks : 
            secretary->visible_scheduled_tasks;
    return sct_list_count_items_by_criteria(list,
        _secretary_predicate_task_is_scheduled_for, params);
}

int sct_secretary_count_tasks_scheduled_for_today(SctSecretary *secretary, 
            bool archived) {
    return sct_secretary_count_tasks_scheduled_for(secretary, time(NULL), archived);
}

void sct_secretary_archive_scheduled_tasks(SctSecretary *secretary) {
    int scheduled_count = sct_list_count_items_by_criteria(secretary->tasks, 
            _secretary_predicate_done_scheduled_task, NULL);
    for (int i = 0; i < scheduled_count; i++) {
        SctTask *task = sct_list_get_nth_item_by_criteria(secretary->tasks, i,
                _secretary_predicate_done_scheduled_task, NULL);
        sct_task_archive(task);
    }
    _secretary_update_sublists(secretary);
}
void sct_secretary_archive_tasks_scheduled_for(SctSecretary *secretary, time_t date) {
    SctList *params = sct_list_new_with(1, &date);
    int scheduled_count = sct_list_count_items_by_criteria(secretary->tasks, 
            _secretary_predicate_done_task_scheduled_for, params);
    for (int i = 0; i < scheduled_count; i++) {
        SctTask *task = sct_list_get_nth_item_by_criteria(secretary->tasks, i,
                _secretary_predicate_done_task_scheduled_for, params);
        sct_task_archive(task);
    }
    _secretary_update_sublists(secretary);
}
void sct_secretary_archive_tasks_scheduled_for_today(SctSecretary *secretary) {
    sct_secretary_archive_tasks_scheduled_for(secretary, time(NULL));
    _secretary_update_sublists(secretary);
}

SctTask *sct_secretary_get_nth_task_scheduled(SctSecretary *secretary, int n, 
            bool archived) {
    SctList *list = archived ? 
            secretary->archived_scheduled_tasks : 
            secretary->visible_scheduled_tasks;
    return sct_list_get_nth_item(list, n);
}

SctTask *sct_secretary_get_nth_task_scheduled_for(SctSecretary *secretary, time_t date, 
        int n, bool archived) {
    SctList *params = sct_list_new_with(2, &archived, &date);
    SctList *list = archived ? 
            secretary->archived_scheduled_tasks : 
            secretary->visible_scheduled_tasks;
    return sct_list_get_nth_item_by_criteria(list,
                n, _secretary_predicate_task_is_scheduled_for, params);
}
SctTask *sct_secretary_get_nth_task_scheduled_for_today(SctSecretary *secretary, int n,
        bool archived) {
    return sct_secretary_get_nth_task_scheduled_for(secretary, time(NULL), n, archived);
}

// WHY ON EARTH HAS IT AN archived ARGUMENT!?!?!?!
int sct_secretary_count_done_tasks(SctSecretary *secretary, bool archived) {
    SctList *params = sct_list_new_with(1, &archived);
    return sct_list_count_items_by_criteria(secretary->tasks,
            _secretary_predicate_task_is_done, params);
}

SctTask *sct_secretary_get_nth_done_task(SctSecretary *secretary, int n, bool archived) {
    SctList *params = sct_list_new_with(1, &archived);
    return sct_list_get_nth_item_by_criteria(secretary->tasks, n,
            _secretary_predicate_task_is_done, params);
}

void sct_secretary_schedule_task(SctSecretary *secretary, SctTask *task, time_t time) {
    sct_task_schedule(task, time);
    _secretary_update_sublists(secretary);
}

void sct_secretary_unschedule_task(SctSecretary *secretary, SctTask *task) {
    sct_task_unschedule(task);
    _secretary_update_sublists(secretary);
}

void sct_secretary_move_task_to_project(SctSecretary *secretary, SctProject *project, 
        SctTask *task) {
    sct_project_add_task(project, task);
    //_secretary_update_sublists(secretary);
}

void sct_secretary_remove_task_from_project(SctSecretary *secretary, SctTask *task) {
    sct_project_remove_task(task->project, task);
    //_secretary_update_sublists(secretary);
}

void sct_secretary_archive_tasks_from_project(SctSecretary *secretary, SctProject *project) {
    sct_project_archive_tasks(project);
    _secretary_update_sublists(secretary);
}

void sct_secretary_archive_task(SctSecretary *secretary, SctTask *task) {
    sct_task_archive(task);
    _secretary_update_sublists(secretary);
}
/* INTERNAL INTERFACE: functions which should never be used by secretary clients
 */

void _secretary_sort_tasks(SctSecretary *secretary) {
    sct_list_sort(secretary->tasks, _secretary_task_compare);
}

void _secretary_update_sublists(SctSecretary *secretary) {
    _secretary_sort_tasks(secretary);

    SctList *tasks = secretary->tasks;
    int first_visible_scheduled_task, first_visible_scheduled_for_today_task,
        last_visible_scheduled_for_today_task, last_visible_scheduled_task,
        first_visible_inbox_task, last_visible_inbox_task, 
        first_archived_scheduled_task, first_archived_scheduled_for_today_task,
        last_archived_scheduled_for_today_task, last_archived_scheduled_task,
        first_archived_inbox_task, last_archived_inbox_task, first_visible_task,
        last_visible_task, first_archived_task, last_archived_task;

    first_visible_scheduled_task = first_visible_scheduled_for_today_task =
        last_visible_scheduled_for_today_task = last_visible_scheduled_task =
        first_visible_inbox_task = last_visible_inbox_task = 
        first_archived_scheduled_task = first_archived_scheduled_for_today_task =
        last_archived_scheduled_for_today_task = last_archived_scheduled_task =
        first_archived_inbox_task = last_archived_inbox_task = 
        first_visible_task = last_visible_task = first_archived_task = 
        last_archived_task = SCT_LIST_ITEM_NOT_FOUND;

    for (int i = 0; i < sct_list_count_items(tasks); i++) {
        SctTask *task = sct_list_get_nth_item(tasks, i);
        time_t today = sct_util_beginning_of_day(time(NULL));
        // Is first visible?
        if (!sct_task_is_archived(task)
                && first_visible_task == SCT_LIST_ITEM_NOT_FOUND) {
                first_visible_task = i;
        } else if (sct_task_is_archived(task)         // Is first archived?
                && first_archived_task == SCT_LIST_ITEM_NOT_FOUND) {
            first_archived_task = i;
        }
        // Is first scheduled?
        if (sct_task_is_scheduled(task)) {
            if (!sct_task_is_archived(task)
                    && first_visible_scheduled_task == SCT_LIST_ITEM_NOT_FOUND) {
                first_visible_scheduled_task = i;
            } else if (sct_task_is_archived(task)
                    && first_archived_scheduled_task == SCT_LIST_ITEM_NOT_FOUND) {
                first_archived_scheduled_task = i;
            }
        }
        // Is first scheduled for today?
        if (sct_task_is_scheduled_for(task, today)) {
            if (!sct_task_is_archived(task)
                    && first_visible_scheduled_for_today_task == SCT_LIST_ITEM_NOT_FOUND) {
                first_visible_scheduled_for_today_task = i;
            } else if (sct_task_is_archived(task)
                    && first_archived_scheduled_for_today_task == SCT_LIST_ITEM_NOT_FOUND) {
                first_archived_scheduled_for_today_task = i;
            }
        }
        // Is first inbox?
        if (sct_task_is_in_inbox(task)) { 
            if (!sct_task_is_archived(task)
                    && first_visible_inbox_task == SCT_LIST_ITEM_NOT_FOUND) {
                first_visible_inbox_task = i;
            } else if (sct_task_is_archived(task)
                    && first_archived_inbox_task == SCT_LIST_ITEM_NOT_FOUND) {
                first_archived_inbox_task = i;
            }
        }

        // Is last visbile?
        if (sct_task_is_archived(task) 
                && first_visible_task != SCT_LIST_ITEM_NOT_FOUND 
                && last_visible_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_visible_task = i;
        } else if (!sct_task_is_archived(task) 
                && first_archived_task != SCT_LIST_ITEM_NOT_FOUND 
                && last_archived_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_archived_task = i;
        }
        // Is last scheduled?
        if ((!sct_task_is_scheduled(task) || sct_task_is_archived(task))
                && first_visible_scheduled_task != SCT_LIST_ITEM_NOT_FOUND 
                && last_visible_scheduled_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_visible_scheduled_task = i;
        } else if (!sct_task_is_scheduled(task) && sct_task_is_archived(task)
                && first_archived_scheduled_task != SCT_LIST_ITEM_NOT_FOUND 
                && last_archived_scheduled_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_archived_scheduled_task = i;
        }
        // Is last scheduled for today?
        if ((!sct_task_is_scheduled_for(task, today) || sct_task_is_archived(task))
                && first_visible_scheduled_for_today_task != SCT_LIST_ITEM_NOT_FOUND 
                && last_visible_scheduled_for_today_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_visible_scheduled_for_today_task = i;
        } else if (!sct_task_is_scheduled_for(task, today) && sct_task_is_archived(task)
                && first_archived_scheduled_for_today_task != SCT_LIST_ITEM_NOT_FOUND 
                && last_archived_scheduled_for_today_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_archived_scheduled_for_today_task = i;
        }
        // Is last inbox?
        if ((!sct_task_is_in_inbox(task) || sct_task_is_archived(task))
                && first_visible_inbox_task != SCT_LIST_ITEM_NOT_FOUND 
                && last_visible_inbox_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_visible_inbox_task = i;
        } else if (!sct_task_is_in_inbox(task) && sct_task_is_archived(task)
                && first_archived_inbox_task != SCT_LIST_ITEM_NOT_FOUND 
                && last_archived_inbox_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_archived_inbox_task = i;
        }
    }
    int start, count;
    // Updating range of visible/archived lists
    if (first_visible_task != SCT_LIST_ITEM_NOT_FOUND) {
        if (last_visible_task == SCT_LIST_ITEM_NOT_FOUND) {
            bool archival = false;
            SctList *params = sct_list_new_with(1, &archival);
            last_visible_task = sct_list_count_items_by_criteria(secretary->tasks,
                _secretary_predicate_task_archival_is, params);
        }
        start = first_visible_task;
        count = last_visible_task-first_visible_task;
    } else {
        start = count = 0;
    }
    sct_sublist_update_range(secretary->visible_tasks, start, count);    

    if (first_archived_task != SCT_LIST_ITEM_NOT_FOUND) {
        if (last_archived_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_archived_task = sct_list_count_items(secretary->tasks);
        }
        start = first_archived_task;
        count = last_archived_task-first_archived_task;
    } else {
        start = count = 0;
    }
    sct_sublist_update_range(secretary->archived_tasks, start, count);    


    // Updating range for visible lists
    if (first_visible_scheduled_task != SCT_LIST_ITEM_NOT_FOUND) {
        if (last_visible_scheduled_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_visible_scheduled_task = sct_secretary_count_tasks(secretary, false);
        }
        start = first_visible_scheduled_task;
        count = last_visible_scheduled_task-first_visible_scheduled_task;
    } else {
        start = count = 0;
    }
    sct_sublist_update_range(secretary->visible_scheduled_tasks, start, count);

    if (first_visible_scheduled_for_today_task != SCT_LIST_ITEM_NOT_FOUND) {
        if (last_visible_scheduled_for_today_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_visible_scheduled_for_today_task = sct_secretary_count_tasks(
                    secretary, false);
        }
        start = first_visible_scheduled_for_today_task;
        count = last_visible_scheduled_for_today_task-first_visible_scheduled_for_today_task;
    } else {
        start = count = 0;
    }
    sct_sublist_update_range(secretary->visible_scheduled_for_today_tasks, start, 
            count);
    if (first_visible_inbox_task != SCT_LIST_ITEM_NOT_FOUND) {
        if (last_visible_inbox_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_visible_inbox_task = sct_secretary_count_all_tasks(secretary);
        }
        start = first_visible_inbox_task;
        count = last_visible_inbox_task-first_visible_inbox_task;
    } else {
        start = count = 0;
    }
    sct_sublist_update_range(secretary->visible_inbox, start, count);

    // Updating range for archived lists
    if (first_archived_scheduled_task != SCT_LIST_ITEM_NOT_FOUND) {
        if (last_archived_scheduled_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_archived_scheduled_task = sct_secretary_count_all_tasks(secretary);
        }
        start = first_archived_scheduled_task;
        count = last_archived_scheduled_task-first_archived_scheduled_task;
    } else {
        start = count = 0;
    }
    sct_sublist_update_range(secretary->archived_scheduled_tasks, start, count);

    if (first_archived_scheduled_for_today_task != SCT_LIST_ITEM_NOT_FOUND) {
        if (last_archived_scheduled_for_today_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_archived_scheduled_for_today_task = 
                    sct_secretary_count_all_tasks(secretary);
        }
        start = first_archived_scheduled_for_today_task;
        count = last_archived_scheduled_for_today_task
            - first_archived_scheduled_for_today_task;
    } else {
        start = count = 0;
    }
    sct_sublist_update_range(secretary->archived_scheduled_for_today_tasks, start, 
            count);
    if (first_archived_inbox_task != SCT_LIST_ITEM_NOT_FOUND) {
        if (last_archived_inbox_task == SCT_LIST_ITEM_NOT_FOUND) {
            last_archived_inbox_task = sct_secretary_count_all_tasks(secretary);
        }
        start = first_archived_inbox_task;
        count = last_archived_inbox_task-first_archived_inbox_task;
    } else {
        start = count = 0;
    }
    sct_sublist_update_range(secretary->archived_inbox, start, count);
}

void _secretary_add_task(SctSecretary *secretary, SctTask *task) {
    sct_list_add_item(secretary->tasks, task);
    if (task->number > secretary->acc) {
        secretary->acc = task->number;
    }
}

/* UtilComparators */
int _secretary_task_compare(const void *p1, const void *p2) {
    const SctTask *task1 = *(SctTask**)p1, *task2 = *(SctTask**)p2;
    return sct_task_compare(task1, task2);
}

/* Predicates */
bool _secretary_predicate_task_is_in_inbox(void *task, SctList *params) {
    bool archived = *(bool*) sct_list_get_nth_item(params, 0);
    return sct_task_is_in_inbox(task) && sct_task_is_archived(task) == archived;
}

bool _secretary_predicate_task_archival_is(void *task, SctList *params) {
    bool archived = *(bool*) sct_list_get_nth_item(params, 0);
    return sct_task_is_archived(task) == archived;
}

bool _secretary_predicate_project_is_named(void *project, SctList *params) {
    const char *name = (const char*) sct_list_get_nth_item(params, 0);
    return strcmp(sct_project_get_name(project), name) == 0;
}

bool _secretary_predicate_task_is_done(void *task, SctList *params) {
    bool archived = params ? *(bool*) sct_list_get_nth_item(params, 0) : false;
    return sct_task_is_done(task) && sct_task_is_archived(task) == archived;
}

bool _secretary_predicate_task_is_scheduled(void *task, SctList *params) {
    bool archived = *(bool*) sct_list_get_nth_item(params, 0);
    return sct_task_is_scheduled(task) && sct_task_is_archived(task) == archived;
}

bool _secretary_predicate_task_is_scheduled_for(void *task, SctList *params) {
    bool archived = *(bool*) sct_list_get_nth_item(params, 0);
    time_t date = *(time_t*) sct_list_get_nth_item(params, 1);
    return sct_task_is_scheduled_for(task, date) && sct_task_is_archived(task) == archived;
}

bool _secretary_predicate_task_is_not_scheduled_for_today(void *task, 
        SctList *params) {
    bool archived = *(bool*) sct_list_get_nth_item(params, 0);
    time_t today = sct_util_beginning_of_day(time(NULL));
    bool not_scheduled = !sct_task_is_scheduled(task);
    bool scheduled_after_today = sct_task_is_scheduled(task) 
            && sct_util_beginning_of_day(sct_task_get_scheduled_date(task)) > today;
    return (not_scheduled || scheduled_after_today) 
            && sct_task_is_archived(task) == archived;
}


bool _secretary_predicate_done_scheduled_task(void *task, SctList *params) {
    return sct_task_is_done(task) && sct_task_is_scheduled(task);
}

bool _secretary_predicate_done_task_scheduled_for(void *task, SctList *params) {
    time_t date = *(time_t*)  sct_list_get_nth_item(params, 0);
    return sct_task_is_done(task) && sct_task_is_scheduled_for(task, date);
}

bool _secretary_predicate_inbox_task_is_done(void *task, SctList *params) {
    // TODO pass bool instead of bool*
    bool archived = params ? *(bool*)sct_list_get_nth_item(params, 0) : false;
    return sct_task_is_in_inbox(task) && sct_task_is_done(task) && sct_task_is_archived(task) == archived;
}

bool _secretary_project_is_not_archived(void *project, SctList *params) {
    return ! sct_project_is_archived(project);
}

void _secretary_sort(SctPublisher *publisher,
        const char *event_name, SctList *params) {
    SctSecretary *secretary =  sct_list_get_nth_item(params, 0);
    _secretary_update_sublists(secretary);
}
