#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "polynomial.h"
#include "extended_value.h"

/* ---------------------------------------
 * Helpers
 * --------------------------------------- */
static Polynomial make_poly(int degree, double *coeffs)
{
    Polynomial p;
    p.degree = degree;
    p.coefficients = coeffs;
    return p;
}

/* ---------------------------------------
 * polynomial_evaluate tests
 * --------------------------------------- */

static void test_value_constant(void **state)
{
    (void)state;

    double c[] = {5.0};
    Polynomial p = make_poly(0, c);

    assert_float_equal(polynomial_evaluate(&p, 10.0), 5.0, 1e-9);
}

static void test_value_linear(void **state)
{
    (void)state;

    double c[] = {1.0, 2.0}; // 2x + 1
    Polynomial p = make_poly(1, c);

    assert_float_equal(polynomial_evaluate(&p, 3.0), 7.0, 1e-9);
}

static void test_value_quadratic(void **state)
{
    (void)state;

    double c[] = {-1.0, 0.0, 1.0}; // x^2 - 1
    Polynomial p = make_poly(2, c);

    assert_float_equal(polynomial_evaluate(&p, 2.0), 3.0, 1e-9);
}

static void test_value_negative_x(void **state)
{
    (void)state;

    double c[] = {0.0, 1.0}; // x
    Polynomial p = make_poly(1, c);

    assert_float_equal(polynomial_evaluate(&p, -3.0), -3.0, 1e-9);
}

static void test_value_zero_polynomial(void **state)
{
    (void)state;

    double c[] = {0.0};
    Polynomial p = make_poly(0, c);

    assert_float_equal(polynomial_evaluate(&p, 100.0), 0.0, 1e-9);
}

/* ---------------------------------------
 * polynomial_limit tests
 * --------------------------------------- */

static void test_limit_finite(void **state)
{
    (void)state;

    double c[] = {1.0, 1.0}; // x + 1
    Polynomial p = make_poly(1, c);

    ExtendedValue ev = polynomial_limit(&p, extended_value_finite(2.0));

    assert_int_equal(ev.type, VALUE_FINITE);
    assert_float_equal(ev.value, 3.0, 1e-9);
}

static void test_limit_positive_infinity_even_degree(void **state)
{
    (void)state;

    double c[] = {-1.0, 0.0, 1.0}; // x^2 - 1
    Polynomial p = make_poly(2, c);

    ExtendedValue ev = polynomial_limit(&p, extended_value_pos_infinity());

    assert_int_equal(ev.type, VALUE_POS_INF);
}

static void test_limit_negative_infinity_even_degree(void **state)
{
    (void)state;

    double c[] = {-1.0, 0.0, 1.0}; // x^2 - 1
    Polynomial p = make_poly(2, c);

    ExtendedValue ev = polynomial_limit(&p, extended_value_neg_infinity());

    assert_int_equal(ev.type, VALUE_POS_INF);
}

static void test_limit_positive_infinity_odd_degree(void **state)
{
    (void)state;

    double c[] = {0.0, 0.0, 1.0}; // x^2 (still even, but positive lead)
    Polynomial p = make_poly(2, c);

    ExtendedValue ev = polynomial_limit(&p, extended_value_pos_infinity());

    assert_int_equal(ev.type, VALUE_POS_INF);
}

static void test_limit_negative_infinity_odd_degree(void **state)
{
    (void)state;

    double c[] = {0.0, 0.0, 1.0}; // x^2
    Polynomial p = make_poly(2, c);

    ExtendedValue ev = polynomial_limit(&p, extended_value_neg_infinity());

    assert_int_equal(ev.type, VALUE_POS_INF);
}

static void test_limit_linear_negative_lead(void **state)
{
    (void)state;

    double c[] = {0.0, -2.0}; // -2x
    Polynomial p = make_poly(1, c);

    ExtendedValue ev_pos = polynomial_limit(&p, extended_value_pos_infinity());
    ExtendedValue ev_neg = polynomial_limit(&p, extended_value_neg_infinity());

    assert_int_equal(ev_pos.type, VALUE_NEG_INF);
    assert_int_equal(ev_neg.type, VALUE_POS_INF);
}

static void test_limit_zero_polynomial(void **state)
{
    (void)state;

    double c[] = {0.0};
    Polynomial p = make_poly(0, c);

    ExtendedValue ev = polynomial_limit(&p, extended_value_pos_infinity());

    assert_int_equal(ev.type, VALUE_FINITE);
    assert_float_equal(ev.value, 0.0, 1e-9);
}

/* ---------------------------------------
 * Test runner
 * --------------------------------------- */

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_value_constant),
        cmocka_unit_test(test_value_linear),
        cmocka_unit_test(test_value_quadratic),
        cmocka_unit_test(test_value_negative_x),
        cmocka_unit_test(test_value_zero_polynomial),

        cmocka_unit_test(test_limit_finite),
        cmocka_unit_test(test_limit_positive_infinity_even_degree),
        cmocka_unit_test(test_limit_negative_infinity_even_degree),
        cmocka_unit_test(test_limit_positive_infinity_odd_degree),
        cmocka_unit_test(test_limit_negative_infinity_odd_degree),
        cmocka_unit_test(test_limit_linear_negative_lead),
        cmocka_unit_test(test_limit_zero_polynomial),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
