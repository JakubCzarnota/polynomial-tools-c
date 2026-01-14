// polynomial.h
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <stddef.h>
#include <stdbool.h>

#include "extended_value.h"
#include "root_array_list.h"
#include "point_array_list.h"
#include "interval_array_list.h"

typedef struct
{
    char *formula;

    Interval range;

    int degree;
    double *coefficients;

    RootArrayList roots;

    PointArrayList extreme_points;

    IntervalArrayList positive_value_intervals;
    IntervalArrayList negative_value_intervals;

    IntervalArrayList monotonic_increasing_intervals;
    IntervalArrayList monotonic_decreasing_intervals;

    PointArrayList inflection_points;

    IntervalArrayList concave_intervals;
    IntervalArrayList convex_intervals;

} Polynomial;

// creation
Polynomial create_polynomial_from_formula(
    const char *formula,
    char *error_msg,
    int error_msg_size);

Polynomial create_polynomial(const double *coefficients, int degree);
Polynomial create_zero_polynomial(void);
Polynomial create_monomial(int degree, double coefficient);
Polynomial create_binomial(int degree, double coefficient);

// lifecycle
Polynomial copy_polynomial(const Polynomial *p);
void free_polynomial(Polynomial *p);

// evaluation & calculus
double polynomial_evaluate(const Polynomial *p, double x);
ExtendedValue polynomial_limit(const Polynomial *p, ExtendedValue approach);
Polynomial polynomial_derivative(const Polynomial *p);

// arithmetic
Polynomial polynomial_add(const Polynomial *p1, const Polynomial *p2);
Polynomial polynomial_subtract(const Polynomial *p1, const Polynomial *p2);
Polynomial polynomial_multiply(const Polynomial *p1, const Polynomial *p2);
Polynomial polynomial_scalar_multiply(const Polynomial *p, double scalar);

Polynomial polynomial_divide(
    const Polynomial *p1,
    const Polynomial *p2,
    Polynomial *rest);

// analysis
void polynomial_find_properties(Polynomial *p);

bool polynomial_is_zero(const Polynomial *p);
bool polynomial_is_integer(const Polynomial *poly);

// formatting
void polynomial_to_string(const Polynomial *p, char *buffer, int buffer_size);

// numeric methods
double newton_raphson_polynomial(
    const Polynomial *p, 
    const Polynomial *prime, 
    double x0, 
    double tol, 
    int max_iter);

#endif // POLYNOMIAL_H
