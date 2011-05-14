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
    return list->items[index];
}

void list_remove(List *list, void *item) {
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

static void list_expand(List *list) {
    list->space *= 2;
    list->items = realloc(list->items, list->space*sizeof(void*));
    if (!list->items) {
        exit(1);
    }
}

