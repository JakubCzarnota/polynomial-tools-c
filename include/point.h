#ifndef POINT_H
#define POINT_H

typedef struct
{
    double x;
    double y;
} Point;

// creation
Point create_point(double x, double y);

// utilities
int point_compare_by_x(const Point *a, const Point *b);

void point_to_string(
    const Point *p,
    char *buffer,
    size_t buffer_size);

#endif // POINT_H