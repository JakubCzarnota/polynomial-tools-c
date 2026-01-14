#ifndef INTERVAL_ARRAY_LIST_H
#define INTERVAL_ARRAY_LIST_H

#include "interval.h"

typedef struct
{
    Interval *values;
    int size;
    int capacity;
} IntervalArrayList;

// lifecycle
void init_interval_array_list(IntervalArrayList *list);

void free_interval_array_list(IntervalArrayList *list);

// modification
void interval_array_list_add(IntervalArrayList *list, Interval interval);

void interval_array_list_remove_last(IntervalArrayList *list);

// utilities
void interval_array_list_to_string(
    const IntervalArrayList *list,
    char *buffer,
    int buffer_size);

void interval_array_list_sort(IntervalArrayList *list);

#endif //INTERVAL_ARRAY_LIST_H