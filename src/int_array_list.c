// double_array_list.c
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "int_array_list.h"

void init_int_array_list(IntArrayList *list)
{
    int *initial_array = malloc(8 * sizeof(int));
    list->values = initial_array;
    list->size = 0;
    list->capacity = 8;
}

void int_array_list_to_string(const IntArrayList *list, char *buffer, int buffer_size)
{
    if (!buffer || buffer_size == 0)
        return;

    int offset = 0;
    for (int i = 0; i < list->size; i++)
    {
        int len;
        if (i == list->size - 1)
            len = snprintf(buffer + offset, buffer_size - offset, "%d", list->values[i]);
        else
            len = snprintf(buffer + offset, buffer_size - offset, "%d, ", list->values[i]);

        if (len >= buffer_size - offset)
            break;
        offset += len;
    }
}

void int_array_list_add_value(IntArrayList *list, int value)
{
    if (list->size >= list->capacity)
    {
        int new_capacity = list->capacity * 2;
        int *new_array = realloc(list->values, new_capacity * sizeof(int));
        if (!new_array)
        {
            perror("realloc failed");
            exit(-1);
        }
        list->values = new_array;
        list->capacity = new_capacity;
    }

    list->values[list->size] = value;
    list->size++;
}

void int_array_list_remove_last_value(IntArrayList *list)
{
    if (list->size <= 0)
        return;

    list->size--;

    if (list->size > 0 && list->capacity / list->size >= 4 && list->capacity > 8)
    {
        int new_capacity = list->capacity / 2;
        int *new_array = realloc(list->values, new_capacity * sizeof(int));
        if (!new_array)
        {
            perror("realloc failed");
            exit(-1);
        }
        list->values = new_array;
        list->capacity = new_capacity;
    }
}

static int cmp_int(const void *a, const void *b)
{
    int da = *(const int *)a;
    int db = *(const int *)b;

    if (da < db)
        return -1;
    if (da > db)
        return 1;
    return 0;
}

void sort_int_array_list(IntArrayList *list)
{
    qsort(list->values, list->size, sizeof(int), cmp_int);
}

void free_int_array_list(IntArrayList *list)
{
    free(list->values);
    list->values = NULL;
    list->size = 0;
    list->capacity = 0;
}