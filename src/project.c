#include <secretary/project.h>

#include <stdlib.h>
#include <string.h>

Project *project_new(const char *name) {
    Project *project = malloc(sizeof(Project));
    project->name = name;
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

void project_remove(Project *project, Task *task) {
    for (int i = 0; i < project->task_count; i++) {
        Task *cursor = project->tasks[i];
        if (cursor->number == task-> number) {
            memmove(project->tasks+i, project->tasks+i, (project->task_count-i+1)*sizeof(Task*));
            project->task_count--;
        }
    }
    task->project = NULL;
}

void project_free(Project *project) {
   for (int i = 0; i < project->task_count; i++) {
       project->tasks[i]->project = NULL;
   }
   free(project);
}
