#ifndef POINT_ARRAY_LIST_H
#define POINT_ARRAY_LIST_H

#include "point.h"

typedef struct
{
    Point *values;
    int size;
    int capacity;
} PointArrayList;

// lifecycle
void init_point_array_list(PointArrayList *list);

void free_point_array_list(PointArrayList *list);

// modification
void point_array_list_add(PointArrayList *list, Point point);

void point_array_list_remove_last(PointArrayList *list);

// utilities
void point_array_list_sort_by_x(PointArrayList *list);

void point_array_list_to_string(
    const PointArrayList *list,
    char *buffer,
    size_t buffer_size);

#endif // POINT_ARRAY_LIST_H