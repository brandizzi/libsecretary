#ifndef _PROJECT_H
# define _PROJECT_H

#include <task.h>

typedef struct {
    const char *name;    
} Project;

Project *project_new(const char *name);

#define project_name(project) ((project)->name)

#endif
