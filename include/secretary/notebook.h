#ifndef _SECRETARY_NOTEBOOK_H
# define _SECRETARY_NOTEBOOK_H

#include <secretary/secretary.h>
#include <secretary/parser.h>

typedef struct {
    char *filename;
    int major_version;
    int minor_version;
    Secretary *secretary;
} Notebook;

Notebook *notebook_new(const char *filename);
Secretary *notebook_get_secretary(Notebook *notebook);
void notebook_save(Notebook *notebook);
void notebook_free(Notebook *notebook);

#endif

