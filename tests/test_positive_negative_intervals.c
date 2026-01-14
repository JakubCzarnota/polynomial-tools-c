#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <math.h>
#include <stdlib.h>

/* ==== headers from your project ==== */
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

/* f(x) = x
 * positive: (0, +inf)
 * negative: (-inf, 0)
 */
static void test_linear_positive_negative(void **state)
{
    (void)state;

    double c[] = {0.0, 1.0};
    Polynomial p = create_polynomial(c, 1);

    polynomial_find_properties(&p);

    IntervalArrayList *pos = &p.positive_value_intervals;
    IntervalArrayList *neg = &p.negative_value_intervals;

    assert_int_equal(pos->size, 1);
    assert_int_equal(neg->size, 1);

    assert_interval(&pos->values[0],
                    VALUE_FINITE, 0.0,
                    VALUE_POS_INF, 0.0);

    assert_interval(&neg->values[0],
                    VALUE_NEG_INF, 0.0,
                    VALUE_FINITE, 0.0);

    free_polynomial(&p);
}

/* f(x) = x^2 - 1
 * roots: -1, 1
 * positive: (-inf,-1), (1,inf)
 * negative: (-1,1)
 */
static void test_quadratic_signs(void **state)
{
    (void)state;

    double c[] = {-1.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 2);

    polynomial_find_properties(&p);

    IntervalArrayList *pos = &p.positive_value_intervals;
    IntervalArrayList *neg = &p.negative_value_intervals;

    assert_int_equal(pos->size, 2);
    assert_int_equal(neg->size, 1);

    assert_interval(&pos->values[0],
                    VALUE_NEG_INF, 0.0,
                    VALUE_FINITE, -1.0);

    assert_interval(&pos->values[1],
                    VALUE_FINITE, 1.0,
                    VALUE_POS_INF, 0.0);

    assert_interval(&neg->values[0],
                    VALUE_FINITE, -1.0,
                    VALUE_FINITE, 1.0);

    free_polynomial(&p);
}

/* f(x) = -x^2
 * negative everywhere except x = 0
 */
static void test_negative_everywhere(void **state)
{
    (void)state;

    double c[] = {0.0, 0.0, -1.0};
    Polynomial p = create_polynomial(c, 2);

    polynomial_find_properties(&p);

    assert_int_equal(p.positive_value_intervals.size, 0);
    assert_int_equal(p.negative_value_intervals.size, 2);

    assert_interval(
        &p.negative_value_intervals.values[0],
        VALUE_NEG_INF, 0.0,
        VALUE_FINITE, 0.0);

    assert_interval(
        &p.negative_value_intervals.values[1],
        VALUE_FINITE, 0.0,
        VALUE_POS_INF, 0.0);

    free_polynomial(&p);
}

/* f(x) = (x - 1)^2
 * zero at x = 1
 * positive everywhere else
 */
static void test_positive_everywhere_except_root(void **state)
{
    (void)state;

    double c[] = {1.0, -2.0, 1.0};
    Polynomial p = create_polynomial(c, 2);

    polynomial_find_properties(&p);

    assert_int_equal(p.negative_value_intervals.size, 0);
    assert_int_equal(p.positive_value_intervals.size, 2);

    assert_interval(&p.positive_value_intervals.values[0],
                    VALUE_NEG_INF, 0.0,
                    VALUE_FINITE, 1.0);

    assert_interval(&p.positive_value_intervals.values[1],
                    VALUE_FINITE, 1.0,
                    VALUE_POS_INF, 0.0);

    free_polynomial(&p);
}

/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_linear_positive_negative),
        cmocka_unit_test(test_quadratic_signs),
        cmocka_unit_test(test_negative_everywhere),
        cmocka_unit_test(test_positive_everywhere_except_root),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
