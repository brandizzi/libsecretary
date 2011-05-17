/**
 * A simple array-based list for our internal purposes.
 */
#ifndef _LIST_H
# define _LIST_H

#import <secretary/util.h>

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

/**
 * Sorts the list using the comparator function. This function should behave 
 * like the function received by the qsort() standard library function - that is,
 * it should receive two pointers and return a number less than, equals or
 * greater than 0 if the second pointed item is respectively smaller than, equal
 * to or greater than the first pointed item.
 */

void list_sort(List *list, UtilComparator comparator);

#endif
