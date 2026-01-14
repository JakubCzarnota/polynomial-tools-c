#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <math.h>

#include "polynomial.h"
#include "root_array_list.h"
#include "root.h"

/* ---------------------------------------
 * Helper functions
 * --------------------------------------- */

static void assert_root_in_list(const RootArrayList *list,
                                int index,
                                double expected_value,
                                int expected_mult)
{
    assert_true(index < list->size);
    const Root *r = &list->values[index];
    assert_double_equal(r->value, expected_value, 1e-6);
    assert_int_equal(r->multiplicity, expected_mult);
}

/* ---------------------------------------
 * Tests for polynomial_find_properties
 * --------------------------------------- */

/* x - 3 */
static void test_single_simple_root(void **state)
{
    (void)state;

    double c[] = {-3.0, 1.0};
    Polynomial p = create_polynomial(c, 1);

    polynomial_find_properties(&p);

    assert_int_equal(p.roots.size, 1);
    assert_root_in_list(&p.roots, 0, 3.0, 1);

    free_polynomial(&p);
}

/* x^2 - 1 = (x - 1)(x + 1) */
static void test_two_simple_roots(void **state)
{
    (void)state;

    double c[] = {-1.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 2);

    polynomial_find_properties(&p);

    assert_int_equal(p.roots.size, 2);

    root_array_list_sort(&p.roots);

    assert_root_in_list(&p.roots, 0, -1.0, 1);
    assert_root_in_list(&p.roots, 1, 1.0, 1);

    free_polynomial(&p);
}

/* (x - 2)^2 = x^2 - 4x + 4 */
static void test_double_root(void **state)
{
    (void)state;

    double c[] = {4.0, -4.0, 1.0};
    Polynomial p = create_polynomial(c, 2);

    polynomial_find_properties(&p);

    assert_int_equal(p.roots.size, 1);
    assert_root_in_list(&p.roots, 0, 2.0, 2);

    free_polynomial(&p);
}

/* (x + 1)^2 (x - 3) = x^3 - x^2 - 5x - 3 */
static void test_mixed_multiplicity_roots(void **state)
{
    (void)state;

    double c[] = {-3.0, -5.0, -1.0, 1.0};
    Polynomial p = create_polynomial(c, 3);

    polynomial_find_properties(&p);

    assert_int_equal(p.roots.size, 2);

    root_array_list_sort(&p.roots);

    assert_root_in_list(&p.roots, 0, -1.0, 2);
    assert_root_in_list(&p.roots, 1, 3.0, 1);

    free_polynomial(&p);
}

/* x^4 - 5x^2 + 6 = (x^2 - 2)(x^2 - 3) */
static void test_irrational_roots(void **state)
{
    (void)state;

    double c[] = {6.0, 0.0, -5.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 4);

    polynomial_find_properties(&p);

    assert_int_equal(p.roots.size, 4);

    root_array_list_sort(&p.roots);

    assert_root_in_list(&p.roots, 0, -sqrt(3.0), 1);
    assert_root_in_list(&p.roots, 1, -sqrt(2.0), 1);
    assert_root_in_list(&p.roots, 2, sqrt(2.0), 1);
    assert_root_in_list(&p.roots, 3, sqrt(3.0), 1);

    free_polynomial(&p);
}

/* x^2 + 1 → no real roots */
static void test_no_real_roots(void **state)
{
    (void)state;

    double c[] = {1.0, 0.0, 1.0};
    Polynomial p = create_polynomial(c, 2);

    polynomial_find_properties(&p);

    assert_int_equal(p.roots.size, 0);

    free_polynomial(&p);
}

/* ---------------------------------------
 * Test runner
 * --------------------------------------- */

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_single_simple_root),
        cmocka_unit_test(test_two_simple_roots),
        cmocka_unit_test(test_double_root),
        cmocka_unit_test(test_mixed_multiplicity_roots),
        cmocka_unit_test(test_no_real_roots),
        cmocka_unit_test(test_irrational_roots)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
