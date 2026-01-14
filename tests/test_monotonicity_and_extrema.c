#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <math.h>
#include <stdlib.h>

#include "polynomial.h"
#include "interval.h"
#include "point.h"

/* ==== helpers ==== */

static void assert_double_eq(double a, double b)
{
    assert_true(fabs(a - b) < 1e-6);
}

static void assert_point(const Point *p, double x, double y)
{
    assert_double_eq(p->x, x);
    assert_double_eq(p->y, y);
}

static void assert_interval(
    const Interval *i,
    ValueType lower_type, double lower_value,
    ValueType upper_type, double upper_value)
{
    assert_int_equal(i->lower_bound.type, lower_type);
    assert_int_equal(i->upper_bound.type, upper_type);

    if (lower_type == VALUE_FINITE)
        assert_double_eq(i->lower_bound.value, lower_value);

    if (upper_type == VALUE_FINITE)
        assert_double_eq(i->upper_bound.value, upper_value);
}

/* =========================================================
   TESTS
   ========================================================= */

/* f(x) = x^3 - 3x
 * max: (-1, 2)
 * min: ( 1,-2)
 */
static void test_extreme_points_cubic(void **state)
{
    (void)state;

    double c[] = {0.0, -3.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 3);

    polynomial_find_properties(&p);

    assert_int_equal(p.extreme_points.size, 2);

    assert_point(&p.extreme_points.values[0], -1.0, 2.0);
    assert_point(&p.extreme_points.values[1], 1.0, -2.0);

    free_polynomial(&p);
}

/* f(x) = x^3 - 3x
 * increasing: (-inf,-1), (1,inf)
 * decreasing: (-1,1)
 */
static void test_monotonicity_cubic(void **state)
{
    (void)state;

    double c[] = {0.0, -3.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 3);

    polynomial_find_properties(&p);

    IntervalArrayList *inc = &p.monotonic_increasing_intervals;
    IntervalArrayList *dec = &p.monotonic_decreasing_intervals;

    assert_int_equal(inc->size, 2);
    assert_int_equal(dec->size, 1);

    assert_interval(&inc->values[0],
                    VALUE_NEG_INF, 0.0,
                    VALUE_FINITE, -1.0);

    assert_interval(&inc->values[1],
                    VALUE_FINITE, 1.0,
                    VALUE_POS_INF, 0.0);

    assert_interval(&dec->values[0],
                    VALUE_FINITE, -1.0,
                    VALUE_FINITE, 1.0);

    free_polynomial(&p);
}

/* f(x) = x^3
 * increasing everywhere
 * no extrema
 */
static void test_monotonic_everywhere(void **state)
{
    (void)state;

    double c[] = {0.0, 0.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 3);

    polynomial_find_properties(&p);

    assert_int_equal(p.extreme_points.size, 0);

    assert_int_equal(p.monotonic_increasing_intervals.size, 1);
    assert_int_equal(p.monotonic_decreasing_intervals.size, 0);

    assert_interval(
        &p.monotonic_increasing_intervals.values[0],
        VALUE_NEG_INF, 0.0,
        VALUE_POS_INF, 0.0);

    free_polynomial(&p);
}

/* f(x) = x^2
 * minimum at (0,0)
 */
static void test_parabola_minimum(void **state)
{
    (void)state;

    double c[] = {0.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 2);

    polynomial_find_properties(&p);

    assert_int_equal(p.extreme_points.size, 1);
    assert_point(&p.extreme_points.values[0], 0.0, 0.0);

    assert_int_equal(p.monotonic_decreasing_intervals.size, 1);
    assert_int_equal(p.monotonic_increasing_intervals.size, 1);

    free_polynomial(&p);
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_extreme_points_cubic),
        cmocka_unit_test(test_monotonicity_cubic),
        cmocka_unit_test(test_monotonic_everywhere),
        cmocka_unit_test(test_parabola_minimum),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
