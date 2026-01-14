// polynomial_compute.c
#include <math.h>

#include "polynomial.h"
#include "extended_value.h"

double newton_raphson_polynomial(const Polynomial *p, const Polynomial *prime, double x0, double tol, int max_iter)
{
    double x = x0;
    for (int i = 0; i < max_iter; i++)
    {
        double fx = polynomial_evaluate(p, x);
        double fpx = polynomial_evaluate(prime, x);

        if (fabs(fpx) < 1e-12)
        {
            // printf("Derivative too small, stopping.\n");
            break;
        }

        double x_next = x - fx / fpx;

        if (fabs(x_next - x) < tol)
        {
            return x_next; // Converged
        }

        x = x_next;
    }
    return x; // Return best guess
}

double polynomial_evaluate(const Polynomial *p, double x)
{
    double result = 0.0;
    for (int i = 0; i <= p->degree; i++)
    {
        result += p->coefficients[i] * pow(x, i);
    }
    return result;
}

ExtendedValue polynomial_limit(const Polynomial *p, ExtendedValue approach)
{
    ExtendedValue result;

    if (approach.type == VALUE_UNDEFINED)
    {
        result.type = VALUE_UNDEFINED;
        return result;
    }

    if (p->degree == 0)
    {
        result.type = VALUE_FINITE;
        result.value = p->coefficients[0];
        return result;
    }

    if (approach.type == VALUE_FINITE)
    {
        result.type = VALUE_FINITE;
        result.value = polynomial_evaluate(p, approach.value);
        return result;
    }

    double leading_coef = p->coefficients[p->degree];

    if (p->degree % 2 == 0)
    {

        result.type = (leading_coef > 0) ? VALUE_POS_INF : VALUE_NEG_INF;
    }
    else
    {
        if (approach.type == VALUE_POS_INF)
            result.type = (leading_coef > 0) ? VALUE_POS_INF : VALUE_NEG_INF;

        if (approach.type == VALUE_NEG_INF)
            result.type = (leading_coef > 0) ? VALUE_NEG_INF : VALUE_POS_INF;
    }

    return result;
}
