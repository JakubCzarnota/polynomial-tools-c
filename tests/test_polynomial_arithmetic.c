#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "polynomial.h"

/* -------------------------------
 * Helper: compare two polynomials
 * ------------------------------- */
static void assert_polynomials_equal(const Polynomial *p, const double *expected, int expected_degree)
{
    assert_int_equal(p->degree, expected_degree);
    assert_non_null(p->coefficients);

    for (int i = 0; i <= expected_degree; i++)
    {
        assert_float_equal(p->coefficients[i], expected[i], 1e-9);
    }
}

/* -------------------------------
 * Test addition: (x + 1) + (x - 1) = 2x
 * ------------------------------- */
static void test_addition(void **state)
{
    (void)state;

    double coeffs1[] = {1.0, 1.0};  // x + 1
    double coeffs2[] = {-1.0, 1.0}; // x - 1

    Polynomial p1 = {.degree = 1, .coefficients = coeffs1};
    Polynomial p2 = {.degree = 1, .coefficients = coeffs2};

    Polynomial sum = polynomial_add(&p1, &p2);

    double expected[] = {0.0, 2.0}; // 2x
    assert_polynomials_equal(&sum, expected, 1);

    free_polynomial(&sum);
}

/* -------------------------------
 * Test subtraction: (x + 1) - (x - 1) = 2
 * ------------------------------- */
static void test_subtraction(void **state)
{
    (void)state;

    double coeffs1[] = {1.0, 1.0};  // x + 1
    double coeffs2[] = {-1.0, 1.0}; // x - 1

    Polynomial p1 = {.degree = 1, .coefficients = coeffs1};
    Polynomial p2 = {.degree = 1, .coefficients = coeffs2};

    Polynomial diff = polynomial_subtract(&p1, &p2);

    double expected[] = {2.0}; // 2
    assert_polynomials_equal(&diff, expected, 0);

    free_polynomial(&diff);
}

/* -------------------------------
 * Test multiplication: (x + 1)*(x - 1) = x^2 - 1
 * ------------------------------- */
static void test_multiplication(void **state)
{
    (void)state;

    double coeffs1[] = {1.0, 1.0};  // x + 1
    double coeffs2[] = {-1.0, 1.0}; // x - 1

    Polynomial p1 = {.degree = 1, .coefficients = coeffs1};
    Polynomial p2 = {.degree = 1, .coefficients = coeffs2};

    Polynomial prod = polynomial_multiply(&p1, &p2);

    double expected[] = {-1.0, 0.0, 1.0}; // x^2 - 1
    assert_polynomials_equal(&prod, expected, 2);

    free_polynomial(&prod);
}

/* -------------------------------
 * Test division: (x^2 - 1) / (x - 1) = x + 1, remainder = 0
 * ------------------------------- */
static void test_division(void **state)
{
    (void)state;

    double coeffs1[] = {-1.0, 0.0, 1.0}; // x^2 - 1
    double coeffs2[] = {-1.0, 1.0};      // x - 1

    Polynomial p1 = {.degree = 2, .coefficients = coeffs1};
    Polynomial p2 = {.degree = 1, .coefficients = coeffs2};

    Polynomial remainder;
    Polynomial quotient = polynomial_divide(&p1, &p2, &remainder);

    double expected_quotient[] = {1.0, 1.0}; // x + 1
    double expected_remainder[] = {0.0};     // 0

    assert_polynomials_equal(&quotient, expected_quotient, 1);
    assert_polynomials_equal(&remainder, expected_remainder, 0);

    free_polynomial(&quotient);
    free_polynomial(&remainder);
}

/* -------------------------------
 * Test runner
 * ------------------------------- */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_addition),
        cmocka_unit_test(test_subtraction),
        cmocka_unit_test(test_multiplication),
        cmocka_unit_test(test_division),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
