// polynoamial_properties.c
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "polynomial.h"
#include "root.h"
#include "extended_value.h"
#include "sturm_sequence.h"
#include "interval.h"
#include "point.h"
#include "int_array_list.h"

static void add_roots(Polynomial *p, RootArrayList roots)
{
    for (int i = 0; i < roots.size; i++)
    {
        root_array_list_add(&p->roots, roots.values[i]);
    }
}

static void slice_intervals_untill_contain_one_root(const SturmSequence *sequence, const Interval interval, IntervalArrayList *interval_list)
{
    double mid_point = (interval.lower_bound.value + interval.upper_bound.value) / 2.0;

    Interval left_interval = create_interval(interval.lower_bound, (ExtendedValue){VALUE_FINITE, mid_point}, interval.lower_inclusive, false);

    int realRootsInLeft = sturm_sequence_count_real_roots_in_interval(sequence, left_interval);

    if ((left_interval.upper_bound.value - left_interval.lower_bound.value) > 0.125 && realRootsInLeft > 1)
        slice_intervals_untill_contain_one_root(sequence, left_interval, interval_list);
    else if (realRootsInLeft >= 1)
        interval_array_list_add(interval_list, left_interval);

    Interval right_interval = create_interval((ExtendedValue){VALUE_FINITE, mid_point}, interval.upper_bound, false, interval.upper_inclusive);

    int realRootsInRight = sturm_sequence_count_real_roots_in_interval(sequence, right_interval);

    if ((right_interval.upper_bound.value - right_interval.lower_bound.value) > 0.125 && realRootsInRight > 1)
        slice_intervals_untill_contain_one_root(sequence, right_interval, interval_list);
    else if (realRootsInRight >= 1)
        interval_array_list_add(interval_list, right_interval);
}

static IntervalArrayList find_root_intervals(SturmSequence *sequence)
{
    IntervalArrayList values;
    init_interval_array_list(&values);

    int total_root_count = sturm_sequence_count_real_roots_in_interval(sequence, create_interval((ExtendedValue){VALUE_NEG_INF, 0}, (ExtendedValue){VALUE_POS_INF, 0}, false, false));

    Interval search_interval;

    double defaultStep = 50;
    int roots_in_interval;

    do
    {

        search_interval = create_interval((ExtendedValue){VALUE_FINITE, -defaultStep}, (ExtendedValue){VALUE_FINITE, defaultStep}, true, true);

        roots_in_interval = sturm_sequence_count_real_roots_in_interval(sequence, search_interval);

        defaultStep *= 2;

    } while (roots_in_interval < total_root_count);

    slice_intervals_untill_contain_one_root(sequence, search_interval, &values);

    return values;
}

static void get_divisors(IntArrayList *divisors_list, int number)
{
    if (number < 0)
        number = -number;

    for (int i = 1; i * i <= number; i++)
    {
        if (number % i == 0)
        {
            int_array_list_add_value(divisors_list, i);

            if (i != number / i)
                int_array_list_add_value(divisors_list, number / i);
        }
    }

    sort_int_array_list(divisors_list);
}

static void find_quadratic_roots(Polynomial *p)
{
    double a = p->coefficients[2];
    double b = p->coefficients[1];
    double c = p->coefficients[0];

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0)
        return;

    if (discriminant == 0)
    {
        Root root = create_root(-b / (2 * a), 2);
        root_array_list_add(&p->roots, root);
        return;
    }

    double sqrt_discriminant = sqrt(discriminant);

    Root root1 = create_root((-b + sqrt_discriminant) / (2 * a), 1);
    Root root2 = create_root((-b - sqrt_discriminant) / (2 * a), 1);

    root_array_list_add(&p->roots, root1);
    root_array_list_add(&p->roots, root2);

    root_array_list_sort(&p->roots);
}

static void find_integer_roots_of_integer_polynomial(Polynomial *p)
{
    IntArrayList constant_term_divisors, leading_coefficient_divisors;

    init_int_array_list(&constant_term_divisors);
    init_int_array_list(&leading_coefficient_divisors);

    get_divisors(&constant_term_divisors, p->coefficients[0]);
    get_divisors(&leading_coefficient_divisors, p->coefficients[p->degree]);

    for (int i = 0; i < constant_term_divisors.size; i++)
    {
        double c = constant_term_divisors.values[i];

        for (int j = 0; j < leading_coefficient_divisors.size; j++)
        {
            double l = leading_coefficient_divisors.values[j];

            double possible_root = c / l;

            if (polynomial_evaluate(p, possible_root) == 0)
                root_array_list_add(&p->roots, create_root(possible_root, 1));

            if (polynomial_evaluate(p, -possible_root) == 0)
                root_array_list_add(&p->roots, create_root(-possible_root, 1));
        }
    }

    free_int_array_list(&constant_term_divisors);
    free_int_array_list(&leading_coefficient_divisors);

    root_array_list_sort(&p->roots);
}

static void find_irrational_roots(Polynomial *p, const Polynomial *derivative)
{
    SturmSequence sequence = create_sturm_sequence(p);

    IntervalArrayList values = find_root_intervals(&sequence);

    for (int i = 0; i < values.size; i++)
    {
        Interval interval = values.values[i];

        int multiplicity = sturm_sequence_count_real_roots_in_interval(&sequence, interval);

        double mid_point = (interval.lower_bound.value + interval.upper_bound.value) / 2.0;

        double root_approx = newton_raphson_polynomial(p, derivative, mid_point, 1e-7, 100);

        root_array_list_add(&p->roots, create_root(root_approx, multiplicity));
    }

    root_array_list_sort(&p->roots);

    free_sturm_sequence(&sequence);
    free_interval_array_list(&values);
}

static void find_roots(Polynomial *p, const Polynomial *derivative)
{
    // if (p->root_count > -1)
    //     return;

    // if (p->roots)
    // {
    //     free(p->roots);
    //     p->roots = NULL;
    // }

    // p->root_count = 0;

    if (p->degree == 0)
        return;

    if (p->degree == 1)
    {
        Root root = create_root(-p->coefficients[0] / p->coefficients[1], 1);

        root_array_list_add(&p->roots, root);
        return;
    }

    if (p->degree == 2)
    {
        find_quadratic_roots(p);
        return;
    }

    if (p->coefficients[0] == 0)
    {
        root_array_list_add(&p->roots, create_root(0.0, 1));

        Polynomial polynomial_to_devide = create_polynomial((double[]){0, 1}, 1);

        Polynomial reduced = polynomial_divide(p, &polynomial_to_devide, NULL);
        Polynomial reduced_derivative = polynomial_derivative(&reduced);

        free_polynomial(&polynomial_to_devide);
        find_roots(&reduced, &reduced_derivative);

        free_polynomial(&reduced_derivative);

        add_roots(p, reduced.roots);

        // sort_roots_by_value(p->roots, p->root_count);
        root_array_list_sort(&p->roots);

        free_polynomial(&reduced);
        return;
    }

    if (polynomial_is_integer(p))
    {
        find_integer_roots_of_integer_polynomial(p);

        if (p->roots.size > 0)
        {
            Polynomial reduced = copy_polynomial(p);

            for (int i = 0; i < p->roots.size; i++)
            {
                double coef = -p->roots.values[i].value;

                Polynomial binomial = create_binomial(1, coef);

                Polynomial result = polynomial_divide(&reduced, &binomial, NULL);

                free_polynomial(&reduced);
                free_polynomial(&binomial);

                reduced = result;
            }

            Polynomial reduced_derivative = polynomial_derivative(&reduced);
            find_roots(&reduced, &reduced_derivative);
            free_polynomial(&reduced_derivative);

            if (reduced.roots.size > 0)
            {
                add_roots(p, reduced.roots);
            }

            free_polynomial(&reduced);

            root_array_list_sort(&p->roots);

            return;
        }
    }

    find_irrational_roots(p, derivative);
}

static void find_extreme_points(Polynomial *p, const Polynomial *first_derivative)
{
    if (p->degree <= 1)
        return;

    for (int i = 0; i < first_derivative->roots.size; i++)
    {
        Root *root = &first_derivative->roots.values[i];
        if (root->multiplicity % 2 == 0)
            continue;

        double x = root->value;
        double y = polynomial_evaluate(p, x);

        point_array_list_add(&p->extreme_points, create_point(x, y));
    }

    point_array_list_sort_by_x(&p->extreme_points);
}

static void find_positive_negative_intervals(Polynomial *p)
{
    ExtendedValue lower_bound = {VALUE_NEG_INF, 0};

    int sig = polynomial_limit(p, lower_bound).type == VALUE_POS_INF ? 1 : -1;

    for (int i = 0; i < p->roots.size; i++)
    {
        Root *root = &p->roots.values[i];

        ExtendedValue upper_bound = {VALUE_FINITE, root->value};

        Interval interval = create_interval(lower_bound, upper_bound, false, false);

        if (sig > 0)
            interval_array_list_add(&p->positive_value_intervals, interval);
        else
            interval_array_list_add(&p->negative_value_intervals, interval);

        if (root->multiplicity % 2 != 0)
            sig = -sig;

        lower_bound = (ExtendedValue){VALUE_FINITE, root->value};
    }

    ExtendedValue upper_bound = {VALUE_POS_INF, 0};

    Interval interval = create_interval(lower_bound, upper_bound, false, false);

    if (sig > 0)
        interval_array_list_add(&p->positive_value_intervals, interval);
    else
        interval_array_list_add(&p->negative_value_intervals, interval);

    interval_array_list_sort(&p->positive_value_intervals);
    interval_array_list_sort(&p->negative_value_intervals);
}

static void find_monotonic_intervals(Polynomial *p)
{
    if (p->degree == 0)
        return;

    ExtendedValue lower_bound = {VALUE_NEG_INF, 0};

    int sig = polynomial_limit(p, lower_bound).type == VALUE_NEG_INF ? 1 : -1;

    for (int i = 0; i < p->extreme_points.size; i++)
    {
        Point *extreme_point = &p->extreme_points.values[i];

        ExtendedValue upper_bound = {VALUE_FINITE, extreme_point->x};

        Interval interval = create_interval(lower_bound, upper_bound,
                                            (lower_bound.type == VALUE_FINITE),
                                            (upper_bound.type == VALUE_FINITE));

        if (sig > 0)
            interval_array_list_add(&p->monotonic_increasing_intervals, interval);
        else
            interval_array_list_add(&p->monotonic_decreasing_intervals, interval);

        sig = -sig;

        lower_bound = (ExtendedValue){VALUE_FINITE, extreme_point->x};
    }

    ExtendedValue upper_bound = {VALUE_POS_INF, 0};

    Interval interval = create_interval(lower_bound, upper_bound,
                                        (lower_bound.type == VALUE_FINITE),
                                        false);

    if (sig > 0)
        interval_array_list_add(&p->monotonic_increasing_intervals, interval);
    else
        interval_array_list_add(&p->monotonic_decreasing_intervals, interval);

    interval_array_list_sort(&p->monotonic_increasing_intervals);
    interval_array_list_sort(&p->monotonic_decreasing_intervals);
}

static void find_range(Polynomial *p)
{
    if (p->degree == 0)
    {
        double const_value = p->coefficients[0];
        p->range = create_interval((ExtendedValue){VALUE_FINITE, const_value}, (ExtendedValue){VALUE_FINITE, const_value}, true, true);
        return;
    }

    if (p->degree % 2 != 0)
    {
        p->range = create_interval((ExtendedValue){VALUE_NEG_INF, 0}, (ExtendedValue){VALUE_POS_INF, 0}, false, false);
        return;
    }

    double leading_coef = p->coefficients[p->degree];

    if (leading_coef > 0)
    {
        double min_value = p->extreme_points.values[0].y;

        for (int i = 1; i < p->extreme_points.size; i++)
        {
            Point *ep = &p->extreme_points.values[i];
            if (ep->y < min_value)
                min_value = ep->y;
        }

        p->range = create_interval((ExtendedValue){VALUE_FINITE, min_value}, (ExtendedValue){VALUE_POS_INF, 0}, true, false);
    }
    else
    {
        double max_value = p->extreme_points.values[0].y;

        for (int i = 1; i < p->extreme_points.size; i++)
        {
            Point *ep = &p->extreme_points.values[i];
            if (ep->y > max_value)
                max_value = ep->y;
        }

        p->range = create_interval((ExtendedValue){VALUE_NEG_INF, 0}, (ExtendedValue){VALUE_FINITE, max_value}, false, true);
    }
}

static void find_inflection_points(Polynomial *p, const Polynomial *second_derivative)
{
    if (p->degree <= 2)
        return;

    for (int i = 0; i < second_derivative->roots.size; i++)
    {
        Root *root = &second_derivative->roots.values[i];

        if (root->multiplicity % 2 == 0)
            continue;

        double x = root->value;
        double y = polynomial_evaluate(p, x);

        point_array_list_add(&p->inflection_points, create_point(x, y));
    }

    point_array_list_sort_by_x(&p->inflection_points);

    // free_polynomial(second_derivative);
}

static void find_concavity_intervals(Polynomial *p, const Polynomial *second_derivative)
{
    if (p->degree <= 2)
        return;

    ExtendedValue lower_bound = {VALUE_NEG_INF, 0};

    int sig = polynomial_limit(second_derivative, lower_bound).type == VALUE_POS_INF ? 1 : -1;

    for (int i = 0; i < p->inflection_points.size; i++)
    {
        Point *inflection_point = &p->inflection_points.values[i];

        ExtendedValue upper_bound = {VALUE_FINITE, inflection_point->x};

        Interval interval = create_interval(lower_bound, upper_bound,
                                            (lower_bound.type == VALUE_FINITE),
                                            (upper_bound.type == VALUE_FINITE));

        if (sig > 0)
            interval_array_list_add(&p->convex_intervals, interval);
        else
            interval_array_list_add(&p->concave_intervals, interval);

        sig = -sig;

        lower_bound = (ExtendedValue){VALUE_FINITE, inflection_point->x};
    }

    ExtendedValue upper_bound = {VALUE_POS_INF, 0};

    Interval interval = create_interval(lower_bound, upper_bound,
                                        (lower_bound.type == VALUE_FINITE),
                                        false);

    if (sig > 0)
        interval_array_list_add(&p->convex_intervals, interval);
    else
        interval_array_list_add(&p->concave_intervals, interval);

    interval_array_list_sort(&p->convex_intervals);
    interval_array_list_sort(&p->concave_intervals);

    // free_polynomial(first_derivative);
    // free_polynomial(second_derivative);
}

void polynomial_find_properties(Polynomial *p)
{

    Polynomial derivative = polynomial_derivative(p);
    Polynomial second_derivative = polynomial_derivative(&derivative);
    Polynomial third_derivative = polynomial_derivative(&second_derivative);

    find_roots(&derivative, &second_derivative);
    find_roots(&second_derivative, &third_derivative);

    find_roots(p, &derivative);
    find_extreme_points(p, &derivative);
    find_positive_negative_intervals(p);
    find_monotonic_intervals(p);
    find_range(p);
    find_inflection_points(p, &second_derivative);
    find_concavity_intervals(p, &second_derivative);

    free_polynomial(&derivative);
    free_polynomial(&second_derivative);
    free_polynomial(&third_derivative);
}