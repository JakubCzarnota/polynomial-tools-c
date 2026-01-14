// root.h
#ifndef ROOT_H
#define ROOT_H

typedef struct
{
    double value;
    int multiplicity;
} Root;

// creation
Root create_root(double value, int multiplicity);

// utilities
int compare_roots_by_value(const Root *a, const Root *b);

void root_to_string(
    const Root *root,
    char *buffer,
    int buffer_size);

#endif // ROOT_H