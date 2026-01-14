#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <math.h>
#include <stdlib.h>

#include "polynomial.h"
#include "interval.h"
#include "point.h"

/* =========================================================
   HELPERS
   ========================================================= */

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

/* f(x) = x^3
 * inflection at (0,0)
 * concave: (-inf,0)
 * convex: (0,inf)
 */
static void test_cubic_inflection(void **state)
{
    (void)state;

    double c[] = {0.0, 0.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 3);

    polynomial_find_properties(&p);

    assert_int_equal(p.inflection_points.size, 1);
    assert_point(&p.inflection_points.values[0], 0.0, 0.0);

    assert_int_equal(p.concave_intervals.size, 1);
    assert_int_equal(p.convex_intervals.size, 1);

    assert_interval(&p.concave_intervals.values[0],
                    VALUE_NEG_INF, 0.0,
                    VALUE_FINITE, 0.0);

    assert_interval(&p.convex_intervals.values[0],
                    VALUE_FINITE, 0.0,
                    VALUE_POS_INF, 0.0);

    free_polynomial(&p);
}

/* f(x) = -x^3
 * inflection at (0,0)
 * convex: (-inf,0)
 * concave: (0,inf)
 */
static void test_negative_cubic_inflection(void **state)
{
    (void)state;

    double c[] = {0.0, 0.0, 0.0, -1.0};
    Polynomial p = create_polynomial(c, 3);

    polynomial_find_properties(&p);

    assert_int_equal(p.inflection_points.size, 1);
    assert_point(&p.inflection_points.values[0], 0.0, 0.0);

    assert_int_equal(p.convex_intervals.size, 1);
    assert_int_equal(p.concave_intervals.size, 1);

    assert_interval(&p.convex_intervals.values[0],
                    VALUE_NEG_INF, 0.0,
                    VALUE_FINITE, 0.0);

    assert_interval(&p.concave_intervals.values[0],
                    VALUE_FINITE, 0.0,
                    VALUE_POS_INF, 0.0);

    free_polynomial(&p);
}

/* f(x) = x^4
 * no inflection values
 * convex everywhere
 */
static void test_convex_everywhere(void **state)
{
    (void)state;

    double c[] = {0.0, 0.0, 0.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 4);

    polynomial_find_properties(&p);

    assert_int_equal(p.inflection_points.size, 0);

    assert_int_equal(p.concave_intervals.size, 0);
    assert_int_equal(p.convex_intervals.size, 1);

    assert_interval(&p.convex_intervals.values[0],
                    VALUE_NEG_INF, 0.0,
                    VALUE_POS_INF, 0.0);

    free_polynomial(&p);
}

/* f(x) = -x^4
 * no inflection values
 * concave everywhere
 */
static void test_concave_everywhere(void **state)
{
    (void)state;

    double c[] = {0.0, 0.0, 0.0, 0.0, -1.0};
    Polynomial p = create_polynomial(c, 4);

    polynomial_find_properties(&p);

    assert_int_equal(p.inflection_points.size, 0);

    assert_int_equal(p.convex_intervals.size, 0);
    assert_int_equal(p.concave_intervals.size, 1);

    assert_interval(&p.concave_intervals.values[0],
                    VALUE_NEG_INF, 0.0,
                    VALUE_POS_INF, 0.0);

    free_polynomial(&p);
}

/* f(x) = x^3 - 3x
 * inflection at (0,0)
 * concave: (-inf,0)
 * convex: (0,inf)
 */
static void test_inflection_with_extrema(void **state)
{
    (void)state;

    double c[] = {0.0, -3.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 3);

    polynomial_find_properties(&p);

    assert_int_equal(p.inflection_points.size, 1);
    assert_point(&p.inflection_points.values[0], 0.0, 0.0);

    assert_int_equal(p.concave_intervals.size, 1);
    assert_int_equal(p.convex_intervals.size, 1);

    assert_interval(&p.concave_intervals.values[0],
                    VALUE_NEG_INF, 0.0,
                    VALUE_FINITE, 0.0);

    assert_interval(&p.convex_intervals.values[0],
                    VALUE_FINITE, 0.0,
                    VALUE_POS_INF, 0.0);

    free_polynomial(&p);
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_cubic_inflection),
        cmocka_unit_test(test_negative_cubic_inflection),
        cmocka_unit_test(test_convex_everywhere),
        cmocka_unit_test(test_concave_everywhere),
        cmocka_unit_test(test_inflection_with_extrema),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
