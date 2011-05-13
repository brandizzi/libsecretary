#ifndef _LIST_H
# define _LIST_H

typedef struct List {
    int space, number_of_items;
    void **items;
} List;

List *list_new();
int list_count_items(List *list);
void list_add_item(List *list, void *item);
void *list_get_nth_item(List *list, int index);

void list_free(List *list);

#endif
