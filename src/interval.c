#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "interval.h"

Interval create_interval(ExtendedValue lower, ExtendedValue upper, bool lower_inclusive, bool upper_inclusive)
{
    Interval interval = {.lower_bound = lower, .lower_inclusive = lower_inclusive, .upper_bound = upper, .upper_inclusive = upper_inclusive};
    return interval;
}

Interval interval_all_real()
{
    return create_interval(extended_value_neg_infinity(), extended_value_pos_infinity(), false, false);
}

void interval_to_string(const Interval *interval,
                        char *buffer,
                        size_t buffer_size)
{
    if (!buffer || buffer_size == 0)
        return;

    if (interval == NULL)
    {
        snprintf(buffer, buffer_size, "NULL Interval");
        return;
    }

    /* Undefined interval */
    if (interval->lower_bound.type == VALUE_UNDEFINED ||
        interval->upper_bound.type == VALUE_UNDEFINED)
    {
        snprintf(buffer, buffer_size, "Undefined Interval");
        return;
    }

    /* Whole real line */
    if (interval->lower_bound.type == VALUE_NEG_INF &&
        interval->upper_bound.type == VALUE_POS_INF)
    {
        snprintf(buffer, buffer_size, "R");
        return;
    }

    /* Empty interval checks */
    if (interval->lower_bound.type == VALUE_POS_INF ||
        interval->upper_bound.type == VALUE_NEG_INF)
    {
        snprintf(buffer, buffer_size, "Empty Interval");
        return;
    }

    if (interval->lower_bound.type == VALUE_FINITE &&
        interval->upper_bound.type == VALUE_FINITE)
    {

        double l = interval->lower_bound.value;
        double u = interval->upper_bound.value;

        if (l > u ||
            (l == u && (!interval->lower_inclusive ||
                        !interval->upper_inclusive)))
        {
            snprintf(buffer, buffer_size, "Empty Interval");
            return;
        }
    }

    if (interval->lower_bound.type == VALUE_FINITE &&
        interval->upper_bound.type == VALUE_FINITE &&
        interval->lower_bound.value == interval->upper_bound.value &&
        interval->lower_inclusive && interval->upper_inclusive)
    {
        char value_str[32];
        extended_value_to_string(&interval->lower_bound,
                                 value_str, sizeof(value_str));
        snprintf(buffer, buffer_size, "{%s}", value_str);
        return;
    }

    const char *lower_bracket = interval->lower_inclusive ? "[" : "(";
    const char *upper_bracket = interval->upper_inclusive ? "]" : ")";

    char lower_str[32];
    char upper_str[32];

    extended_value_to_string(&interval->lower_bound,
                             lower_str, sizeof(lower_str));
    extended_value_to_string(&interval->upper_bound,
                             upper_str, sizeof(upper_str));

    snprintf(buffer, buffer_size,
             "%s%s, %s%s",
             lower_bracket, lower_str,
             upper_str, upper_bracket);
}

int compare_intervals(const Interval *a, const Interval *b)
{
    int cmp = compare_extended_values(&a->lower_bound, &b->lower_bound);

    if (cmp != 0)
        return cmp;

    int cmp2 = compare_extended_values(&a->upper_bound, &b->upper_bound);

    return cmp2;
}
