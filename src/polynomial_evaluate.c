// polynomial_evaluate.c
#include <math.h>

#include "polynomial.h"

static int is_integer(double x)
{
    return fabs(x - round(x)) < 1e-9;
}

bool polynomial_is_zero(const Polynomial *p)
{
    return (p->degree == 0 && p->coefficients[0] == 0);
}

bool polynomial_is_integer(const Polynomial *poly)
{
    for (int i = 0; i <= poly->degree; i++)
    {
        if (!is_integer(poly->coefficients[i]))
            return false;
    }

    return true;
}