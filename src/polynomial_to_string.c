// polynomial_to_string.c
#include <stdio.h>
#include <string.h>

#include "polynomial.h"
#include "string_utils.h"

static double coefficient_to_string(const Polynomial *p, int degree, char *buffer, int buffer_size)
{
    if (!buffer || buffer_size == 0)
        return 0;

    double coef = p->coefficients[degree];

    if (coef == 0)
    {
        buffer[0] = '\0';
    }
    else if (degree == 0)
    {
        if (coef < 0)
            snprintf(buffer, buffer_size, "- %g", -coef);
        else
            snprintf(buffer, buffer_size, "%g", coef);
    }
    else if (degree == 1)
    {
        if (coef == 1)
            snprintf(buffer, buffer_size, "x");
        else if (coef == -1)
            snprintf(buffer, buffer_size, "- x");
        else
            snprintf(buffer, buffer_size, "%gx", coef);
    }
    else
    {
        if (coef == 1)
            snprintf(buffer, buffer_size, "x^%d", degree);
        else if (coef == -1)
            snprintf(buffer, buffer_size, "- x^%d", degree);
        else if (coef < 0)
            snprintf(buffer, buffer_size, "- %gx^%d", -coef, degree);
        else
            snprintf(buffer, buffer_size, "%gx^%d", coef, degree);
    }

    return coef;
}

void polynomial_to_string(const Polynomial *p, char *buffer, int buffer_size)
{
    if (!buffer || buffer_size <= 0)
        return;

    buffer[0] = '\0';

    if (polynomial_is_zero(p))
    {
        snprintf(buffer, buffer_size, "0");
        return;
    }

    for (int i = p->degree; i >= 0; i--)
    {
        char term[32];
        double coef = coefficient_to_string(p, i, term, sizeof(term));

        if (coef == 0)
            continue;

        size_t len = strlen(buffer);

        if (len == 0)
        {
            snprintf(buffer, buffer_size, "%s", term);
        }
        else if (coef > 0)
        {
            snprintf(buffer + len,
                     buffer_size - len,
                     " + %s", term);
        }
        else
        {
            snprintf(buffer + len,
                     buffer_size - len,
                     " %s", term);
        }
    }
}
