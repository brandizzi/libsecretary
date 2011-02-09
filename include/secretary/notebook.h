#ifndef _SECRETARY_NOTEBOOK_H
# define _SECRETARY_NOTEBOOK_H

#include <secretary/secretary.h>

typedef struct {
    const char *filename;
    int major_version;
    int minor_version;
    Secretary *secretary;
} Notebook;

Notebook *notebook_new(const char *filename);
#define notebook_get_secretary(notebook) ((notebook)->secretary)
void notebook_save(Notebook *notebook);
void notebook_free(Notebook *notebook);

#endif

