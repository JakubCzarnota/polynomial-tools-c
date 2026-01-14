#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "polynomial.h"

/* -------------------------------
 * Test: simple quadratic
 * f(x) = x^2 + 2x + 1
 * coefficients[n] = x^n
 * ------------------------------- */
static void test_create_quadratic(void **state)
{
    (void)state;

    Polynomial p = create_polynomial_from_formula("x^2 + 2x + 1", NULL, 0);

    assert_int_equal(p.degree, 2);
    assert_non_null(p.coefficients);

    /* coefficients: [1, 2, 1] = constant, x^1, x^2 */
    assert_float_equal(p.coefficients[0], 1.0, 1e-9); // constant
    assert_float_equal(p.coefficients[1], 2.0, 1e-9); // x^1
    assert_float_equal(p.coefficients[2], 1.0, 1e-9); // x^2

    free_polynomial(&p);
}

/* -------------------------------
 * Test: linear polynomial
 * f(x) = -3x + 5
 * ------------------------------- */
static void test_create_linear(void **state)
{
    (void)state;

    Polynomial p = create_polynomial_from_formula("-3x + 5", NULL, 0);

    assert_int_equal(p.degree, 1);
    assert_non_null(p.coefficients);

    /* coefficients: [5, -3] = constant, x^1 */
    assert_float_equal(p.coefficients[0], 5.0, 1e-9);  // constant
    assert_float_equal(p.coefficients[1], -3.0, 1e-9); // x^1

    free_polynomial(&p);
}

/* -------------------------------
 * Test: constant polynomial
 * f(x) = 7
 * ------------------------------- */
static void test_create_constant(void **state)
{
    (void)state;

    Polynomial p = create_polynomial_from_formula("7", NULL, 0);

    assert_int_equal(p.degree, 0);
    assert_non_null(p.coefficients);
    assert_float_equal(p.coefficients[0], 7.0, 1e-9); // constant term

    free_polynomial(&p);
}

/* -------------------------------
 * Test: invalid input
 * ------------------------------- */
static void test_invalid_formula(void **state)
{
    (void)state;

    char error_msg[128] = "";

    Polynomial p = create_polynomial_from_formula("this is invalid", error_msg, sizeof(error_msg));

    /* adapt depending on your design */
    assert_true(error_msg[0]); // Expecting an error message

    free_polynomial(&p);
}

/* -------------------------------
 * Test runner
 * ------------------------------- */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_quadratic),
        cmocka_unit_test(test_create_linear),
        cmocka_unit_test(test_create_constant),
        cmocka_unit_test(test_invalid_formula),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
