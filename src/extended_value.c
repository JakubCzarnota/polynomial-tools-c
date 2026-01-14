// extended_value.c
#include <stdio.h>
#include "extended_value.h"

ExtendedValue extended_value_finite(double value)
{
    ExtendedValue ev = {.type = VALUE_FINITE, .value = value};
    return ev;
}

ExtendedValue extended_value_pos_infinity()
{
    ExtendedValue ev = {.type = VALUE_POS_INF, .value = 1};
    return ev;
}

ExtendedValue extended_value_neg_infinity()
{
    ExtendedValue ev = {.type = VALUE_NEG_INF, .value = -1};
    return ev;
}

ExtendedValue extended_value_undefined(void)
{
    ExtendedValue ev = {.type = VALUE_UNDEFINED, .value = 0};
    return ev;
}

void extended_value_to_string(const ExtendedValue *ev, char *buffer, size_t buffer_size)
{
    if (!buffer || buffer_size == 0)
        return;

    switch (ev->type)
    {
    case VALUE_FINITE:
        snprintf(buffer, buffer_size, "%g", ev->value == 0 ? 0 : ev->value);
        break;
    case VALUE_POS_INF:
        snprintf(buffer, buffer_size, "+Inf");
        break;
    case VALUE_NEG_INF:
        snprintf(buffer, buffer_size, "-Inf");
        break;
    case VALUE_UNDEFINED:
        snprintf(buffer, buffer_size, "NaN");
        break;
    default:
        snprintf(buffer, buffer_size, "Unknown Value Type");
        break;
    }
}

int compare_extended_values(const ExtendedValue *a, const ExtendedValue *b)
{
    if (a->type == VALUE_UNDEFINED || b->type == VALUE_UNDEFINED)
        return 0;

    if (a->type == VALUE_FINITE && b->type == VALUE_FINITE)
    {
        if (a->value < b->value)
            return -1;
        else if (a->value > b->value)
            return 1;
        else
            return 0;
    }

    if (a->type == VALUE_NEG_INF)
    {
        if (b->type == VALUE_NEG_INF)
            return 0;
        else
            return -1;
    }

    if (a->type == VALUE_POS_INF)
    {
        if (b->type == VALUE_POS_INF)
            return 0;
        else
            return 1;
    }

    return 0; // should not reach here
}
