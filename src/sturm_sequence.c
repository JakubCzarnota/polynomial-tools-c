#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "sturm_sequence.h"
#include "polynomial.h"

SturmSequence create_sturm_sequence(const Polynomial *p)
{
    SturmSequence sequence;
    sequence.count = 0;
    sequence.polynomials = NULL;

    Polynomial p0 = copy_polynomial(p);
    Polynomial p1 = polynomial_derivative(p);

    sequence.count = 2;
    sequence.polynomials = malloc(sequence.count * sizeof(Polynomial));
    sequence.polynomials[0] = p0;
    sequence.polynomials[1] = p1;

    while (sequence.polynomials[sequence.count - 1].degree > 0)
    {
        Polynomial remainder;
        Polynomial result = polynomial_divide(&sequence.polynomials[sequence.count - 2],
                                              &sequence.polynomials[sequence.count - 1],
                                              &remainder);

        if (polynomial_is_zero(&remainder))
        {
            free_polynomial(&remainder);
            break;
        }

        Polynomial negative_remainder = polynomial_scalar_multiply(&remainder, -1.0);

        free_polynomial(&remainder);
        free_polynomial(&result);

        sequence.count++;
        sequence.polynomials = realloc(sequence.polynomials, sequence.count * sizeof(Polynomial));
        sequence.polynomials[sequence.count - 1] = negative_remainder;
    }

    return sequence;
}

static int sign_of_polynomial_at(const Polynomial *p, ExtendedValue x)
{

    switch (x.type)
    {
    case VALUE_FINITE:
    {
        double val = polynomial_evaluate(p, x.value);
        if (val > 0)
            return 1;
        if (val < 0)
            return -1;
        return 0;
    }
    case VALUE_POS_INF:
    case VALUE_NEG_INF:
    {
        ExtendedValue limit = polynomial_limit(p, x);

        if (limit.type == VALUE_POS_INF)
            return 1;

        if (limit.type == VALUE_NEG_INF)
            return -1;

        if (limit.type == VALUE_FINITE)
        {
            if (limit.value > 0)
                return 1;
            if (limit.value < 0)
                return -1;
        }

        return 0;
    }
    default:
        return 0;
    }
}

static int count_sign_changes(const SturmSequence *sequence, ExtendedValue x)
{
    int sign_changes = 0;
    int previous_sign = sign_of_polynomial_at(&sequence->polynomials[0], x);

    for (int i = 1; i < sequence->count; i++)
    {
        int current_sign = sign_of_polynomial_at(&sequence->polynomials[i], x);

        if (current_sign != 0 && previous_sign != 0 && current_sign != previous_sign)
        {
            sign_changes++;
        }

        if (current_sign != 0)
        {
            previous_sign = current_sign;
        }
    }

    return sign_changes;
}

int sturm_sequence_count_real_roots_in_interval(const SturmSequence *sequence, Interval interval)
{
    int sign_changes_a = count_sign_changes(sequence, interval.lower_bound);
    int sign_changes_b = count_sign_changes(sequence, interval.upper_bound);

    return sign_changes_a - sign_changes_b;
}

void free_sturm_sequence(SturmSequence *sequence)
{
    for (int i = 0; i < sequence->count; i++)
    {
        free_polynomial(&sequence->polynomials[i]);
    }
    free(sequence->polynomials);
    sequence->polynomials = NULL;
    sequence->count = 0;
}
