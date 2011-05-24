#include <secretary/task.h>
#include <secretary/project.h>
#include <secretary/util.h>
#include <stdlib.h>

#include <secretary/_internal/secretary.h>

#define SECONDS_IN_DAY (60*60*24)

Task *task_new(int number, const char *description) {
    Task *task = malloc(sizeof(Task));
    task->number = number;
    task->description = util_copy_string(description);
    task->project = NULL;
    task->scheduled = false;
    task->archived = false;
    task->done = false;
    task->secretary = NULL;
    return task;
}

int task_get_number(Task *task) {
    return task->number;
}

const char *task_get_description(Task *task) {
    return task->description;
}

void task_set_description(Task *task, const char *description) {
    free(task->description);
    task->description = util_copy_string(description);
}

Project *task_get_project(Task *task) {
    return task->project;
}

void task_set_project(Task *task, Project *project) {
    project_add_task(project, task);
}

bool task_has_project(Task *task) {
    return task->project != NULL;
}

bool task_is_in_project(Task *task, struct Project *project) {
    return task->project == project;
}

void task_unset_project(Task *task) {
    if (task->project == NULL) return;
    project_remove_task(task->project, task);
}

bool task_is_in_inbox(Task *task) {
    return task->project == NULL && !task->scheduled;
}

void task_schedule(Task *task, struct tm date) {
    bool was_in_inbox = task_is_in_inbox(task),
        was_scheduled = task_is_scheduled(task);
    task->scheduled = true;
    task->scheduled_for = date;
    // For optimization of secretary
    if (was_in_inbox) {
        _secretary_unregister_from_inbox(task->secretary, task);
    }
    if (!was_scheduled) {
        _secretary_register_in_scheduled(task->secretary, task);
    } else {
        _secretary_sort_scheduled_tasks(task->secretary, task);
    }
}

struct tm task_get_scheduled_date(Task *task) {
    return task->scheduled_for;
}

bool task_is_scheduled(Task *task) {
    return task->scheduled;
}

bool task_is_scheduled_for(Task *task, struct tm date) {
    time_t scheduled = mktime(&task->scheduled_for),
           compared = mktime(&date);
    long scheduled_date = scheduled/SECONDS_IN_DAY, // removing hours, minutes etc.
         compared_date = compared/SECONDS_IN_DAY;
    return task_is_scheduled(task) && scheduled_date <= compared_date;
}

void task_unschedule(Task *task) {
    task->scheduled = false;
    // For secretary optimization
    _secretary_unregister_from_scheduled(task->secretary, task);
    _secretary_register_in_inbox(task->secretary, task);
}

void task_mark_as_done(Task *task) {
    task->done = true;
}

void task_unmark_as_done(Task *task) {
    bool was_archived = task->archived;
    task->done = false;
    task->archived = false;
    // For secretary optimization
    if (was_archived) {
        _secretary_switch_list_in_inbox_perspective(task->secretary, task);
        _secretary_switch_list_in_scheduled_perspective(task->secretary, task);
    }
}

void task_switch_done_status(Task *task) {
    task->done = !task->done;
}

bool task_is_done(Task *task) {
    return task->done;
}

void task_archive(Task *task) {
    bool was_archived = task->archived;
    task->archived = task->done && true;
    if (!was_archived) {
        _secretary_switch_list_in_inbox_perspective(task->secretary, task);
        _secretary_switch_list_in_scheduled_perspective(task->secretary, task);
    }
}

bool task_is_archived(Task *task) {
    return task->archived;
}

void task_free(Task *task) {
    if (task_get_project(task) != NULL) {
        project_remove_task(task_get_project(task), task);
    }
    free(task->description);
    free(task);
}

/* THE FOLLOWING FUNCTIONS SHOULD NOT BE USED BY EXTERNAL CLIENTS */

int _task_compare_by_date(const void *p1, const void* p2) {
    Task *task1 = *(Task**)p1, *task2 = *(Task**)p2;
    return mktime(&(task1->scheduled_for)) - mktime(&(task2->scheduled_for));
}
