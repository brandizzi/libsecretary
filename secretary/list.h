/**
 * libsecretary: a C library for managing to-do lists
 * Copyright (C) 2011  Adam Victor Nazareth Brandizzi <brandizzi@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * You can get the latest version of this file at 
 * http://bitbucket.org/brandizzi/libsecretary/
 */
/**
 * A simple array-based list for our internal purposes.
 */
#ifndef _SCT_LIST_H
# define _SCT_LIST_H

#include <stdbool.h>
#include <secretary/util.h>

#define SCT_LIST_ITEM_NOT_FOUND (-1)

typedef struct SctList {
    int space, number_of_items;
    void **items;
    struct SctList *superlist;
} SctList;

SctList *sct_list_new();
SctList *sct_list_new_with(int count, ...);
int sct_list_count_items(SctList *list);
/**
 * Removes the first occurence of item in the list.
 */
void sct_list_add_item(SctList *list, void *item);
void *sct_list_get_nth_item(SctList *list, int index);
void sct_list_remove_item(SctList *list, void *item);

void sct_list_extend(SctList *dest_list, SctList *source_list);

/**
 * Frees the memory used by the list. It does not free the memory used by
 * the list items, however.
 */
void sct_list_free(SctList *list);

/**
 * Sorts the list using the comparator function. This function should behave 
 * like the function received by the qsort() standard library function - that is,
 * it should receive two pointers and return a number less than, equals or
 * greater than 0 if the second pointed item is respectively smaller than, equal
 * to or greater than the first pointed item.
 */
void sct_list_sort(SctList *list, SctUtilComparator comparator);

/**
 * Returns the nth item which satisfies the predicate function. If there is no
 * such item, returns NULL.
 */
typedef bool (*SctListPredicate)(void *item, SctList *params);
void *sct_list_get_nth_item_by_criteria(SctList *list, int index, 
            SctListPredicate predicate, SctList *params);
int sct_list_count_items_by_criteria(SctList *list, SctListPredicate predicate,
        SctList *params);
int sct_list_get_nth_item_index_by_criteria(SctList *list, int index,
            SctListPredicate predicate, SctList *params, int start_search_from_index);

/**
 * Creates a sublist - a list which just points to a section of another, greater
 * list. This is a read only list - you should not update it.
 */
SctList *sct_sublist_new(SctList *superlist, int start, int count);
void sct_sublist_update_range(SctList *sublist, int start, int count);
void sct_sublist_free(SctList *sublist);

#endif
