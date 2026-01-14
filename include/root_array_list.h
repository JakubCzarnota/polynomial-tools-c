#ifndef ROOT_ARRAY_LIST_H
#define ROOT_ARRAY_LIST_H

#include <stddef.h>
#include <stdbool.h>

#include "root.h"

typedef struct
{
    Root *values;
    int size;
    int capacity;
} RootArrayList;

// lifecycle
void init_root_array_list(RootArrayList *list);
void free_root_array_list(RootArrayList *list);

// modification
void root_array_list_add(RootArrayList *list, Root value);
void root_array_list_remove_last(RootArrayList *list);

// utilities
void root_array_list_sort(RootArrayList *list);

void root_array_list_to_string(
    const RootArrayList *list,
    char *buffer,
    int buffer_size);

#endif // ROOT_ARRAY_LIST_H
