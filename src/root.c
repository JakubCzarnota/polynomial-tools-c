#include <stdio.h>
#include <stdlib.h>
#include "root.h"

Root create_root(double value, int multiplicity)
{
    Root root = {.value = value, .multiplicity = multiplicity};

    return root;
}

void root_to_string(const Root *root, char *buffer, int buffer_size)
{
    if (root->multiplicity == 1)
        snprintf(buffer, buffer_size, "%g", root->value == 0 ? 0 : root->value);
    else
        snprintf(buffer, buffer_size, "%g (multiplicity: %d)", root->value == 0 ? 0 : root->value, root->multiplicity);
}

int compare_roots_by_value(const Root *a, const Root *b)
{
    if (a->value < b->value)
        return -1;
    else if (a->value > b->value)
        return 1;
    else
        return 0;
}
