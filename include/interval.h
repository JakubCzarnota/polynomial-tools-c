#ifndef INTERVAL_H
#define INTERVAL_H

#include <stdbool.h>
#include "extended_value.h"

typedef struct
{
    ExtendedValue lower_bound;
    ExtendedValue upper_bound;
    bool lower_inclusive;
    bool upper_inclusive;
} Interval;

// creation
Interval create_interval(
    ExtendedValue lower,
    ExtendedValue upper,
    bool lower_inclusive,
    bool upper_inclusive);

Interval interval_all_real(void);

// utilities
int compare_intervals(const Interval *a, const Interval *b);

void interval_to_string(const Interval *interval,
                        char *buffer,
                        size_t buffer_size);

#endif // INTERVAL_H