#ifndef _SECRETARY_NOTEBOOK_H
# define _SECRETARY_NOTEBOOK_H

#include <secretary/secretary.h>

typedef struct {
} Notebook;

Notebook *notebook_new(const char *filename);
Secretary *notebook_get_secretary(Notebook *notebook);
void notebook_free(Notebook *notebook);

#endif

