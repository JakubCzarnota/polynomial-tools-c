#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "polynomial.h"

/* ---------------------------------------
 * Helper: assert derivative
 * --------------------------------------- */
static void assert_poly(const Polynomial *p,
                        const double *expected,
                        int expected_degree)
{
    assert_int_equal(p->degree, expected_degree);

    for (int i = 0; i <= expected_degree; i++)
    {
        assert_float_equal(p->coefficients[i], expected[i], 1e-9);
    }
}

/* ---------------------------------------
 * Tests
 * --------------------------------------- */

static void test_derivative_constant(void **state)
{
    (void)state;

    double c[] = {5.0}; // 5
    Polynomial p = {.degree = 0, .coefficients = c};

    Polynomial d = polynomial_derivative(&p);

    double expected[] = {0.0}; // 0
    assert_poly(&d, expected, 0);

    free_polynomial(&d);
}

static void test_derivative_linear(void **state)
{
    (void)state;

    double c[] = {1.0, 3.0}; // 3x + 1
    Polynomial p = {.degree = 1, .coefficients = c};

    Polynomial d = polynomial_derivative(&p);

    double expected[] = {3.0}; // 3
    assert_poly(&d, expected, 0);

    free_polynomial(&d);
}

static void test_derivative_quadratic(void **state)
{
    (void)state;

    double c[] = {1.0, 0.0, 2.0}; // 2x^2 + 1
    Polynomial p = {.degree = 2, .coefficients = c};

    Polynomial d = polynomial_derivative(&p);

    double expected[] = {0.0, 4.0}; // 4x
    assert_poly(&d, expected, 1);

    free_polynomial(&d);
}

static void test_derivative_cubic(void **state)
{
    (void)state;

    double c[] = {-1.0, 0.0, 3.0, 2.0}; // 2x^3 + 3x^2 - 1
    Polynomial p = {.degree = 3, .coefficients = c};

    Polynomial d = polynomial_derivative(&p);

    double expected[] = {0.0, 6.0, 6.0}; // 6x^2 + 6x
    assert_poly(&d, expected, 2);

    free_polynomial(&d);
}

static void test_derivative_zero_polynomial(void **state)
{
    (void)state;

    double c[] = {0.0};
    Polynomial p = {.degree = 0, .coefficients = c};

    Polynomial d = polynomial_derivative(&p);

    double expected[] = {0.0};
    assert_poly(&d, expected, 0);

    free_polynomial(&d);
}

/* ---------------------------------------
 * Test runner
 * --------------------------------------- */

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_derivative_constant),
        cmocka_unit_test(test_derivative_linear),
        cmocka_unit_test(test_derivative_quadratic),
        cmocka_unit_test(test_derivative_cubic),
        cmocka_unit_test(test_derivative_zero_polynomial),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
