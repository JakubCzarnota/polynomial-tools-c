// polynomial_arithmetic.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "polynomial.h"

static void trim_coefficients(Polynomial *p);

static void normalize_polynomial(Polynomial *p)
{
    if (!p || p->degree < 0)
        return;

    while (p->degree >= 0 &&
           fabs(p->coefficients[p->degree]) < 1e-9)
    {
        p->degree--;
    }

    if (p->degree < 0)
    {
        p->degree = 0;
        p->coefficients[0] = 0.0;
    }

    trim_coefficients(p);
}

static void trim_coefficients(Polynomial *p)
{
    while (p->coefficients[p->degree] == 0 && p->degree > 0)
    {
        p->degree--;
    }

    double *new_coefficients = NULL;

    if (p->degree == 0 && p->coefficients[0] == 0)
    {
        free(p->coefficients);
        new_coefficients = calloc(1, sizeof(double));
    }
    else
        new_coefficients = realloc(p->coefficients, (p->degree + 1) * sizeof(double));

    if (!new_coefficients)
    {
        perror("realloc failed");
        exit(EXIT_FAILURE);
    }

    // free(p->coefficients);
    p->coefficients = new_coefficients;
}

Polynomial polynomial_add(const Polynomial *p1, const Polynomial *p2)
{
    if (p1 == NULL || p2 == NULL)
    {
        if (p1 != NULL)
            return copy_polynomial(p1);
        else if (p2 != NULL)
            return copy_polynomial(p2);
        else
            return create_zero_polynomial();
    }

    int max_degree = (p1->degree > p2->degree) ? p1->degree : p2->degree;

    double *coefficients = calloc(max_degree + 1, sizeof(double));

    if (!coefficients)
    {
        perror("calloc failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= max_degree; i++)
    {
        double coef1 = (i <= p1->degree) ? p1->coefficients[i] : 0;
        double coef2 = (i <= p2->degree) ? p2->coefficients[i] : 0;
        coefficients[i] = coef1 + coef2;
    }

    Polynomial result = create_polynomial(coefficients, max_degree);

    trim_coefficients(&result);

    free(coefficients);
    return result;
}

Polynomial add_constant(const Polynomial *p, double c)
{
    Polynomial newPolynomial = copy_polynomial(p);

    for (int i = 0; i <= newPolynomial.degree; i++)
    {
        newPolynomial.coefficients[i] *= c;
    }

    trim_coefficients(&newPolynomial);

    return newPolynomial;
}

Polynomial polynomial_subtract(const Polynomial *p1, const Polynomial *p2)
{
    int max_degree = (p1->degree > p2->degree) ? p1->degree : p2->degree;

    double *coefficients = calloc(max_degree + 1, sizeof(double));

    if (!coefficients)
    {
        perror("calloc failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= max_degree; i++)
    {
        double coef1 = (i <= p1->degree) ? p1->coefficients[i] : 0;
        double coef2 = (i <= p2->degree) ? p2->coefficients[i] : 0;
        coefficients[i] = coef1 - coef2;
    }

    Polynomial result = create_polynomial(coefficients, max_degree);

    trim_coefficients(&result);

    free(coefficients);
    return result;
}

Polynomial polynomial_multiply(const Polynomial *p1, const Polynomial *p2)
{
    int degree = p1->degree + p2->degree;
    double *coefficients = calloc(degree + 1, sizeof(double));

    for (int i = 0; i <= p1->degree; i++)
    {
        for (int j = 0; j <= p2->degree; j++)
        {
            coefficients[i + j] += p1->coefficients[i] * p2->coefficients[j];
        }
    }

    Polynomial result = create_polynomial(coefficients, degree);
    free(coefficients);

    trim_coefficients(&result);

    return result;
}

Polynomial polynomial_scalar_multiply(const Polynomial *p, double scalar)
{
    if (scalar == 0.0)
    {
        double zero = 0.0;
        return create_polynomial(&zero, 0);
    }

    Polynomial nnewPoly = copy_polynomial(p);

    for (int i = 0; i <= nnewPoly.degree; i++)
    {
        nnewPoly.coefficients[i] *= scalar;
    }

    return nnewPoly;
}

Polynomial polynomial_divide(const Polynomial *p1, const Polynomial *p2, Polynomial *rest)
{
    if (p1 == NULL || p2 == NULL)
        return create_zero_polynomial();

    if (polynomial_is_zero(p1) || polynomial_is_zero(p2))
    {
        if (rest != NULL)
            *rest = create_zero_polynomial();
        return copy_polynomial(p1);
    }

    Polynomial result = create_zero_polynomial();
    Polynomial poly_to_devide = copy_polynomial(p1);

    while (poly_to_devide.degree >= p2->degree &&
           !polynomial_is_zero(&poly_to_devide))
    {
        // print_polynomial(&poly_to_devide);

        Polynomial monomial = create_monomial(poly_to_devide.degree - p2->degree, poly_to_devide.coefficients[poly_to_devide.degree] / p2->coefficients[p2->degree]);

        // print_polynomial(&monomial);

        Polynomial add_result = polynomial_add(&result, &monomial);

        // print_polynomial(&add_result);

        free_polynomial(&result);

        result = add_result;

        Polynomial poly_to_subtract = polynomial_multiply(&monomial, p2);

        // print_polynomial(&poly_to_subtract);

        free_polynomial(&monomial);

        Polynomial subtract_result = polynomial_subtract(&poly_to_devide, &poly_to_subtract);

        // print_polynomial(&subtract_result);

        free_polynomial(&poly_to_devide);
        free_polynomial(&poly_to_subtract);

        poly_to_devide = subtract_result;
        normalize_polynomial(&poly_to_devide);
    }

    // print_polynomial(&poly_to_devide);

    if (rest)
        *rest = poly_to_devide;
    else
        free_polynomial(&poly_to_devide);

    return result;
}