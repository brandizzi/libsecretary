#include <secretary/project.h>
#include <secretary/util.h>
#include <stdlib.h>
#include <string.h>

Project *project_new(const char *name) {
    Project *project = malloc(sizeof(Project));
    project->name = util_copy_string(name);
    project->task_count = 0;
    return project;
}

void project_add_task(Project *project, Task *task) {
    task_unset_project(task);
    project->tasks[project->task_count++] = task;
    task->project = project;
}

Task *project_get_task(Project *project, int number) {
    for (int i = 0; i < project->task_count; i++) {
        Task *task = project->tasks[i];
        if (task->number == number) {
            return task;
        }
    }
    return NULL;
}

Task *project_get_nth_task(Project *project, int n, bool archived) {
    for (int i = 0; i < project->task_count; i++) {
        Task *task = project->tasks[i];
        if (task_is_archived(task) == archived && n-- == 0) {
            return task;
        }
    }
    return NULL;
}

int project_count_tasks(Project *project, bool archived) {
    int counter = 0;
    for (int i = 0; i < project->task_count; i++) {
        Task *task = project->tasks[i];
        if (task_is_archived(task) == archived) {
            counter++;
        }
    }
    return counter;
}

void project_remove_task(Project *project, Task *task) {
    for (int i = 0; i < project->task_count; i++) {
        Task **cursor = project->tasks+i;
        if (*cursor == task) {
            Task **destination = cursor;
            Task **origin = cursor+1;
            memmove(destination, origin, (project->task_count-i-1)*sizeof(Task*));
            project->task_count--;
            task->project = NULL;
            break;
        }
    }
}

const char* project_get_name(Project *project) {
    return project->name;
}
void project_set_name(Project *project, const char *name) {
    free(project->name);
    project->name = util_copy_string(name);
}

void project_free(Project *project) {
   for (int i = 0; i < project->task_count; i++) {
       project->tasks[i]->project = NULL;
   }
   free(project->name);
   free(project);
}
