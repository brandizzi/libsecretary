#include <secretary/notebook.h>

#include <stdlib.h>

Notebook *notebook_new(const char *filename) {
    return malloc(sizeof(Notebook));
}

Secretary *notebook_get_secretary(Notebook *notebook) {
    return secretary_new();
}
void notebook_free(Notebook *notebook) {
    secretary_free(notebook_get_secretary(notebook));
    free(notebook);
}
