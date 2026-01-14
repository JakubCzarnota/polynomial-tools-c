// point_array_list.c
#include <stdlib.h>
#include <stdio.h>

#include "point_array_list.h"

void init_point_array_list(PointArrayList *list)
{
    Point *initial_array = malloc(8 * sizeof(Point));
    list->values = initial_array;
    list->size = 0;
    list->capacity = 8;
}

void point_array_list_to_string(const PointArrayList *list, char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size == 0)
        return;

    int offset = 0;
    for (int i = 0; i < list->size; i++)
    {
        char point_str[128];
        point_to_string(&list->values[i], point_str, sizeof(point_str));

        size_t len;
        if (i == list->size - 1)
            len = snprintf(buffer + offset, buffer_size - offset, "%s", point_str);
        else
            len = snprintf(buffer + offset, buffer_size - offset, "%s, ", point_str);

        if (len >= buffer_size - offset)
            break;
        offset += len;
    }
}

void point_array_list_add(PointArrayList *list, Point point)
{
    if (list->size >= list->capacity)
    {
        int new_capacity = list->capacity * 2;
        Point *new_array = realloc(list->values, new_capacity * sizeof(Point));
        if (!new_array)
        {
            perror("realloc failed");
            exit(-1);
        }
        list->values = new_array;
        list->capacity = new_capacity;
    }

    list->values[list->size] = point;
    list->size++;
}

void point_array_list_remove_last(PointArrayList *list)
{
    if (list->size <= 0)
        return;

    list->size--;

    if (list->size > 0 && list->capacity / list->size >= 4 && list->capacity > 8)
    {
        int new_capacity = list->capacity / 2;
        Point *new_array = realloc(list->values, new_capacity * sizeof(Point));
        if (!new_array)
        {
            perror("realloc failed");
            exit(-1);
        }
        list->values = new_array;
        list->capacity = new_capacity;
    }
}

void point_array_list_sort_by_x(PointArrayList *list)
{
    qsort(list->values, list->size, sizeof(Point),
          (int (*)(const void *, const void *))point_compare_by_x);
}

void free_point_array_list(PointArrayList *list)
{
    free(list->values);
    list->values = NULL;
    list->size = 0;
    list->capacity = 0;
}