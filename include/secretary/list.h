
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
#ifndef _LIST_H
# define _LIST_H

#import <stdbool.h>
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

/**
 * Returns the nth item which satisfies the predicate function. If there is no
 * such item, returns NULL.
 */
typedef bool (*ListPredicate)(void *item, void **params);
void *list_get_nth_item_by_criteria(List *list, int index, 
            ListPredicate predicate, void **params);
int list_count_items_by_criteria(List *list, ListPredicate predicate,
        void **params);

/**
 * Creates a sublist - a list which just points to a section of another, greater
 * list. This is a read only list - you should not update it.
 */
List *sublist_new(List *superlist, int start, int count);
void sublist_free(List *sublist);

#endif
