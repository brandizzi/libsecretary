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
#include <secretary/list.h>
#include <secretary/util.h>
#include <stdlib.h>
#include <string.h>

#define SCT_LIST_INITIAL_SPACE 256

static void sct_list_expand(SctList *list);

SctList *sct_list_new() {
    SctList *list = malloc(sizeof(SctList));
    list->items = malloc(SCT_LIST_INITIAL_SPACE*sizeof(void*));
    list->space = SCT_LIST_INITIAL_SPACE;
    list->number_of_items = 0;
    return list;
}
int sct_list_count_items(SctList *list) {
    return list->number_of_items;
}

void sct_list_add_item(SctList *list, void *item) {
    if (list->space - list->number_of_items < 4) {
        sct_list_expand(list);
    }
    list->items[list->number_of_items++] = item;
}

void *sct_list_get_nth_item(SctList *list, int index) {
    if (index < list->number_of_items) {
        return list->items[index];
    } else {
        return NULL;
    }
}

void sct_list_remove_item(SctList *list, void *item) {
    for (int i = 0; i < list->number_of_items; i++) {   
        if (list->items[i] == item) {
            void **found = list->items+i;
            memmove(found, found+1, (list->number_of_items-i)*sizeof(void*));
            list->number_of_items--;
            break;
        }
    }
}

void sct_list_extend(SctList *dest_list, SctList *source_list) {
    for (int i = 0; i < sct_list_count_items(source_list); i++) {   
        sct_list_add_item(dest_list, sct_list_get_nth_item(source_list, i));
    }
}

void sct_list_free(SctList *list) {
    free(list->items);
    free(list);
}

void sct_list_sort(SctList *list, UtilComparator comparator) {
    qsort(list->items, list->number_of_items, sizeof(void*), comparator);
}

void *sct_list_get_nth_item_by_criteria(SctList *list, int index, 
            SctListPredicate predicate, void **params) {
    int counter = 0;
    for (int i = 0; i < list->number_of_items; i++) {
        if (predicate(list->items[i], params)) {
            if (counter == index) return list->items[i];
            counter++;
        }
    }
    return NULL;
}

int sct_list_count_items_by_criteria(SctList *list, SctListPredicate predicate,
        void **params) {
    int counter = 0;
    for (int i = 0; i < list->number_of_items; i++) {
        if (predicate(list->items[i], params)) counter++;
    }
    return counter;
}

int sct_list_get_nth_item_index_by_criteria(SctList *list, int index,
            SctListPredicate predicate, void **params, 
            int start_search_from_index) {
    int counter = 0;
    for (int i = start_search_from_index; i < list->number_of_items; i++) {
        if (predicate(list->items[i], params)) {
            if (counter == index) return i;
            counter++;
        }
    }
    return SCT_LIST_ITEM_NOT_FOUND;
}


SctList *sct_sublist_new(SctList *superlist, int start, int count) {
    SctList *sublist = malloc(sizeof(SctList));
    sublist->superlist = superlist;
    sublist->items = superlist->items+start;
    sublist->space = 0;
    sublist->number_of_items = count;
    return sublist;
}

void sct_sublist_update_range(SctList *sublist, int start, int count) {
    sublist->items = sublist->superlist->items+start;
    sublist->number_of_items = count;
}

void sct_sublist_free(SctList *sublist) {
    free(sublist);
}


/* PRIVATE FUNCTIONS */

static void sct_list_expand(SctList *list) {
    list->space *= 2;
    list->items = realloc(list->items, list->space*sizeof(void*));
    if (!list->items) {
        exit(1);
    }
}

