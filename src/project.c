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

void project_add(Project *project, Task *task) {
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

Task *project_get_nth_task(Project *project, int n) {
    for (int i = 0; i < project->task_count; i++) {
        if (!task_is_done(project->tasks[i])) {
            if (n-- == 0) return project->tasks[i];
        }
    }
    return NULL;
}

void project_remove(Project *project, Task *task) {
    for (int i = 0; i < project->task_count; i++) {
        Task *cursor = project->tasks[i];
        if (cursor == task) {
            Task **destination = project->tasks+i;
            Task **origin = project->tasks+(i+1);
            memmove(destination, origin, (project->task_count-i-1)*sizeof(Task*));
            project->task_count--;
        }
    }
    task->project = NULL;
}

const char* project_get_name(Project *project) {
    return project->name;
}
void project_set_name(Project *project, const char *name) {
    free(project->name);
    project->name = util_copy_string(name);
}

int project_count_task(Project *project) {
    return project->task_count;
}

void project_free(Project *project) {
   for (int i = 0; i < project->task_count; i++) {
       project->tasks[i]->project = NULL;
   }
   free(project->name);
   free(project);
}
