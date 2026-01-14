#include <stdio.h>
#include <stdlib.h>
#include "point.h"

Point create_point(double x, double y)
{
    Point p = {
        .x = x,
        .y = y};
    return p;
}

int point_compare_by_x(const Point *a, const Point *b)
{
    if (a->x < b->x)
        return -1;
    else if (a->x > b->x)
        return 1;
    else
        return 0;
}

void point_to_string(const Point *p, char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size == 0)
        return;

    snprintf(buffer, buffer_size, "(%g, %g)", p->x == 0 ? 0 : p->x, p->y == 0 ? 0 : p->y);
}