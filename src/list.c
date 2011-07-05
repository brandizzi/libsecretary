
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

#define LIST_INITIAL_SPACE 256

static void list_expand(List *list);

List *list_new() {
    List *list = malloc(sizeof(List));
    list->items = malloc(LIST_INITIAL_SPACE*sizeof(void*));
    list->space = LIST_INITIAL_SPACE;
    list->number_of_items = 0;
    return list;
}
int list_count_items(List *list) {
    return list->number_of_items;
}

void list_add_item(List *list, void *item) {
    if (list->space - list->number_of_items < 4) {
        list_expand(list);
    }
    list->items[list->number_of_items++] = item;
}

void *list_get_nth_item(List *list, int index) {
    if (index < list->number_of_items) {
        return list->items[index];
    } else {
        return NULL;
    }
}

void list_remove_item(List *list, void *item) {
    for (int i = 0; i < list->number_of_items; i++) {   
        if (list->items[i] == item) {
            void **found = list->items+i;
            memmove(found, found+1, (list->number_of_items-i)*sizeof(void*));
            list->number_of_items--;
            break;
        }
    }
}

void list_free(List *list) {
    free(list->items);
    free(list);
}

void list_sort(List *list, UtilComparator comparator) {
    qsort(list->items, list->number_of_items, sizeof(void*), comparator);
}

/* PRIVATE FUNCTIONS */

static void list_expand(List *list) {
    list->space *= 2;
    list->items = realloc(list->items, list->space*sizeof(void*));
    if (!list->items) {
        exit(1);
    }
}

