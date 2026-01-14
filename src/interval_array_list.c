// interval_array_list.c
#include <stdlib.h>
#include <stdio.h>

#include "interval_array_list.h"

void init_interval_array_list(IntervalArrayList *list)
{
    Interval *initial_array = malloc(8 * sizeof(Interval));
    list->values = initial_array;
    list->size = 0;
    list->capacity = 8;
}

void interval_array_list_to_string(const IntervalArrayList *list, char *buffer, int buffer_size)
{
    if (!buffer || buffer_size == 0)
        return;

    int offset = 0;
    for (int i = 0; i < list->size; i++)
    {
        char interval_str[256];
        interval_to_string(&list->values[i], interval_str, sizeof(interval_str));

        int len;
        if (i == list->size - 1)
            len = snprintf(buffer + offset, buffer_size - offset, "%s", interval_str);
        else
            len = snprintf(buffer + offset, buffer_size - offset, "%s, ", interval_str);

        if (len >= buffer_size - offset)
            break;
        offset += len;
    }
}

void interval_array_list_add(IntervalArrayList *list, Interval interval)
{
    if (list->size >= list->capacity)
    {
        int new_capacity = list->capacity * 2;
        Interval *new_array = realloc(list->values, new_capacity * sizeof(Interval));
        if (!new_array)
        {
            perror("realloc failed");
            exit(-1);
        }
        list->values = new_array;
        list->capacity = new_capacity;
    }

    list->values[list->size] = interval;
    list->size++;
}

void interval_array_list_remove_last(IntervalArrayList *list)
{
    if (list->size <= 0)
        return;

    list->size--;

    if (list->size > 0 && list->capacity / list->size >= 4 && list->capacity > 8)
    {
        int new_capacity = list->capacity / 2;
        Interval *new_array = realloc(list->values, new_capacity * sizeof(Interval));
        if (!new_array)
        {
            perror("realloc failed");
            exit(-1);
        }
        list->values = new_array;
        list->capacity = new_capacity;
    }
}

void interval_array_list_sort(IntervalArrayList *list)
{
    qsort(list->values, list->size, sizeof(Interval),
          (int (*)(const void *, const void *))compare_intervals);
}

void free_interval_array_list(IntervalArrayList *list)
{
    free(list->values);
    list->values = NULL;
    list->size = 0;
    list->capacity = 0;
}
