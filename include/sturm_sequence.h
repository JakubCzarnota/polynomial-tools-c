#ifndef STURM_SEQUENCE_H
#define STURM_SEQUENCE_H

#include <stddef.h>
#include <math.h>

#include "polynomial.h"
#include "interval.h"

typedef struct
{
    Polynomial *polynomials;
    int count;
} SturmSequence;

// lifecycle
SturmSequence create_sturm_sequence(const Polynomial *p);
void free_sturm_sequence(SturmSequence *sequence);

// analysis
int sturm_sequence_count_real_roots_in_interval(
    const SturmSequence *sequence,
    Interval interval);

#endif // STURM_SEQUENCE_H