#include <stdlib.h>
#include <stdio.h>

#include "root_array_list.h"

#define ROOT_ARRAY_LIST_INITIAL_CAPACITY 8

void init_root_array_list(RootArrayList *list)
{
    list->values = malloc(ROOT_ARRAY_LIST_INITIAL_CAPACITY * sizeof(Root));
    if (!list->values)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    list->size = 0;
    list->capacity = ROOT_ARRAY_LIST_INITIAL_CAPACITY;
}

void root_array_list_add(RootArrayList *list, Root value)
{
    int root_index = -1;

    for (int i = 0; i < list->size; i++)
    {
        if (list->values[i].value == value.value)
        {
            root_index = i;
            break;
        }
    }

    if (root_index >= 0)
    {
        list->values[root_index].multiplicity++;
        return;
    }

    if (list->size == list->capacity)
    {
        size_t new_capacity = list->capacity * 2;
        Root *new_array = realloc(list->values, new_capacity * sizeof(Root));
        if (!new_array)
        {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
        list->values = new_array;
        list->capacity = new_capacity;
    }

    list->values[list->size++] = value;
}

void root_array_list_remove_last(RootArrayList *list)
{
    if (list->size == 0)
        return;

    list->size--;

    if (list->capacity > ROOT_ARRAY_LIST_INITIAL_CAPACITY &&
        list->size <= list->capacity / 4)
    {
        size_t new_capacity = list->capacity / 2;
        Root *new_array = realloc(list->values, new_capacity * sizeof(Root));
        if (!new_array)
        {
            perror("realloc failed");
            exit(EXIT_FAILURE);
        }
        list->values = new_array;
        list->capacity = new_capacity;
    }
}

static int cmp_root(const void *a, const void *b)
{
    const Root *ra = a;
    const Root *rb = b;

    if (ra->value < rb->value)
        return -1;
    if (ra->value > rb->value)
        return 1;
    return 0;
}

void root_array_list_sort(RootArrayList *list)
{
    qsort(list->values, list->size, sizeof(Root), cmp_root);
}

void root_array_list_to_string(const RootArrayList *list, char *buffer, int buffer_size)
{
    if (!buffer || buffer_size == 0)
        return;

    int offset = 0;
    for (int i = 0; i < list->size; i++)
    {
        char point_str[128];
        root_to_string(&list->values[i], point_str, sizeof(point_str));

        int len;
        if (i == list->size - 1)
            len = snprintf(buffer + offset, buffer_size - offset, "%s", point_str);
        else
            len = snprintf(buffer + offset, buffer_size - offset, "%s, ", point_str);

        if (len >= buffer_size - offset)
            break;
        offset += len;
    }
}

void free_root_array_list(RootArrayList *list)
{
    free(list->values);
    list->values = NULL;
    list->size = 0;
    list->capacity = 0;
}
