#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <cmocka.h>

#include "sturm_sequence.h"
#include "polynomial.h"

/* ---------------------------------
 * Helpers
 * --------------------------------- */
static Interval whole_real_line(void)
{
    Interval i;
    i.lower_bound.type = VALUE_NEG_INF;
    i.upper_bound.type = VALUE_POS_INF;
    return i;
}

/* ---------------------------------
 * Test: x^2 - 1 → two real roots
 * --------------------------------- */
static void test_sturm_two_real_roots(void **state)
{
    (void)state;

    Polynomial p = create_polynomial_from_formula("x^2 - 1", NULL, 0);
    SturmSequence seq = create_sturm_sequence(&p);

    Interval interval = whole_real_line();
    int roots = sturm_sequence_count_real_roots_in_interval(&seq, interval);

    assert_int_equal(roots, 2);

    free_sturm_sequence(&seq);
    free_polynomial(&p);
}

/* ---------------------------------
 * Test: x^2 + 1 → zero real roots
 * --------------------------------- */
static void test_sturm_no_real_roots(void **state)
{
    (void)state;

    Polynomial p = create_polynomial_from_formula("x^2 + 1", NULL, 0);
    SturmSequence seq = create_sturm_sequence(&p);

    Interval interval = whole_real_line();
    int roots = sturm_sequence_count_real_roots_in_interval(&seq, interval);

    assert_int_equal(roots, 0);

    free_sturm_sequence(&seq);
    free_polynomial(&p);
}

/* ---------------------------------
 * Test: x^3 - x → three real roots
 * --------------------------------- */
static void test_sturm_three_real_roots(void **state)
{
    (void)state;

    Polynomial p = create_polynomial_from_formula("x^3 - x", NULL, 0);
    SturmSequence seq = create_sturm_sequence(&p);

    Interval interval = whole_real_line();
    int roots = sturm_sequence_count_real_roots_in_interval(&seq, interval);

    assert_int_equal(roots, 3);

    free_sturm_sequence(&seq);
    free_polynomial(&p);
}

/* ---------------------------------
 * Test: linear polynomial
 * --------------------------------- */
static void test_sturm_linear_polynomial(void **state)
{
    (void)state;

    Polynomial p = create_polynomial_from_formula("x - 5", NULL, 0);
    SturmSequence seq = create_sturm_sequence(&p);

    Interval interval = whole_real_line();
    int roots = sturm_sequence_count_real_roots_in_interval(&seq, interval);

    assert_int_equal(roots, 1);

    free_sturm_sequence(&seq);
    free_polynomial(&p);
}

/* ---------------------------------
 * Test: no roots in sub-interval
 * --------------------------------- */
static void test_sturm_interval_subset(void **state)
{
    (void)state;

    Polynomial p = create_polynomial_from_formula("x^2 - 1", NULL, 0);
    SturmSequence seq = create_sturm_sequence(&p);

    Interval interval;
    interval.lower_bound.type = VALUE_FINITE;
    interval.lower_bound.value = 2.0;
    interval.upper_bound.type = VALUE_FINITE;
    interval.upper_bound.value = 10.0;

    int roots = sturm_sequence_count_real_roots_in_interval(&seq, interval);

    assert_int_equal(roots, 0);

    free_sturm_sequence(&seq);
    free_polynomial(&p);
}

/* ---------------------------------
 * Test runner
 * --------------------------------- */
int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_sturm_two_real_roots),
        cmocka_unit_test(test_sturm_no_real_roots),
        cmocka_unit_test(test_sturm_three_real_roots),
        cmocka_unit_test(test_sturm_linear_polynomial),
        cmocka_unit_test(test_sturm_interval_subset),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
