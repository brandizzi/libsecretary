#include <project.h>

#include <stdlib.h>

Project *project_new(const char *name) {
    Project *project = malloc(sizeof(Project));
    project->name = name;
    return project;
}

