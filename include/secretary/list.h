/**
 * A simple array-based list for our internal purposes.
 */
#ifndef _LIST_H
# define _LIST_H

typedef struct List {
    int space, number_of_items;
    void **items;
} List;

List *list_new();
int list_count_items(List *list);
/**
 * Removes the first occurence of item in the list.
 */
void list_add_item(List *list, void *item);
void *list_get_nth_item(List *list, int index);
void list_remove_item(List *list, void *item);

/**
 * Frees the memory used by the list. It does not free the memory used by
 * the list items, however.
 */
void list_free(List *list);

#endif
