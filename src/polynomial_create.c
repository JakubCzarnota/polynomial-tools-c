// polynomial_create.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "polynomial.h"
#include "string_utils.h"

enum
{
    STATE_START,
    STATE_COEFFICIENT_SIGN,
    STATE_COEFFICIENT_VALUE,
    STATE_VARIABLE,
    STATE_EXPONENT_VALUE,
    STATE_END,
    STATE_OTHER
} formula_checking_states;

Polynomial create_zero_polynomial()
{
    double coeffs[1] = {0.0};
    return create_polynomial(coeffs, 0);
}

Polynomial create_monomial(int degree, double coefficient)
{
    double *coefficients = calloc(degree + 1, sizeof(double));
    coefficients[degree] = coefficient;
    Polynomial poly = create_polynomial(coefficients, degree);
    free(coefficients);
    return poly;
}

Polynomial create_binomial(int degree, double coefficient)
{
    double *coefficients = calloc(degree + 1, sizeof(double));
    coefficients[degree] = 1;
    coefficients[0] = coefficient;
    Polynomial poly = create_polynomial(coefficients, degree);
    free(coefficients);
    return poly;
}

Polynomial create_polynomial_from_formula(const char *formula, char *error_msg, int error_msg_size)
{
    char *formula_copy = copy_string(formula);
    remove_whitespace(formula_copy);

    int len = strlen(formula_copy);

    char prevous_char = '\0';
    char ch = '\0';

    if (len == 0)
    {
        free(formula_copy);
        Polynomial p = {0};

        if (error_msg && error_msg_size > 0)
            snprintf(error_msg, error_msg_size, "Empty formula.");

        return p;
    }

    if (formula_copy[0] == '^')
    {
        free(formula_copy);
        Polynomial p = {0};

        if (error_msg && error_msg_size > 0)
            snprintf(error_msg, error_msg_size, "Invalid start of formula.");

        return p;
    }

    if (formula_copy[len - 1] == '+' || formula_copy[len - 1] == '-' || formula_copy[len - 1] == '^')
    {
        free(formula_copy);
        Polynomial p = {0};

        if (error_msg && error_msg_size > 0)
            snprintf(error_msg, error_msg_size, "Invalid end of formula.");

        return p;
    }

    for (int i = 0; i < len; i++)
    {
        ch = formula_copy[i];

        if (!(isdigit((unsigned char)ch) || ch == '+' || ch == '-' || ch == 'x' || ch == 'X' || ch == '^' || ch == '.') || (ch == 'x' && (prevous_char == 'x' || prevous_char == '^')) || (prevous_char == '^' && (ch == '+' || ch == '-' || ch == '^')) || (prevous_char == '+' && ch == '^') || (prevous_char == '-' && ch == '^'))
        {
            free(formula_copy);
            Polynomial p = {0};

            if (error_msg && error_msg_size > 0)
                snprintf(error_msg, error_msg_size, "Invalid character '%c' in formula.", ch);

            return p;
        }
        prevous_char = ch;
    }

    int degree = 0;

    int state = STATE_START;

    for (int i = 0; i < len; i++)
    {
        ch = formula_copy[i];

        switch (state)
        {
        case STATE_START:
        case STATE_OTHER:
            if (ch == 'x' || ch == 'X')
            {
                state = STATE_EXPONENT_VALUE;

                if (degree < 1)
                    degree = 1;
            }
            break;

        case STATE_EXPONENT_VALUE:
        {
            char buf[16];
            int j = 0;

            if (ch != '^')
            {
                if (degree < 1)
                    degree = 1;

                break;
            }

            i++;

            while (i < len && j < 16 - 1)
            {
                ch = formula_copy[i];

                if (!isdigit((unsigned char)ch))
                {
                    break;
                }

                buf[j] = ch;
                buf[j + 1] = '\0';

                i++;
                j++;
            }
            char *endptr;
            int current_pow = (int)strtol(buf, &endptr, 10);

            if (current_pow > degree)
                degree = current_pow;

            state = STATE_OTHER;
        }
        break;
        }
    }

    double *coefficients = calloc(degree + 1, sizeof(double));

    char buf[128];

    int j = 0;

    state = STATE_COEFFICIENT_VALUE;
    int sign = 1;

    double current_coefficient = 1.0;
    int current_exponent = 0;

    for (int i = 0; i < len; i++)
    {
        char ch = formula_copy[i];
        switch (state)
        {
        case STATE_COEFFICIENT_VALUE:
            if (ch == '+')
            {
                sign = 1;
            }
            else if (ch == '-')
            {
                sign = -1;
            }
            else if (isdigit((unsigned char)ch) || ch == '.')
            {
                j = 0;
                while (i < len && (isdigit((unsigned char)formula_copy[i]) || formula_copy[i] == '.'))
                {
                    buf[j++] = formula_copy[i++];
                }
                buf[j] = '\0';

                if (j > 0 && buf[0] == '+' && buf[1] == '\0')
                    current_coefficient = 1.0 * sign;
                else if (j > 0 && buf[0] == '-' && buf[1] == '\0')
                    current_coefficient = -1.0 * sign;
                else
                    current_coefficient = strtod(buf, NULL) * sign;

                state = STATE_EXPONENT_VALUE;
                i--;
            }
            else if (ch == 'x' || ch == 'X')
            {
                current_coefficient = sign;
                state = STATE_EXPONENT_VALUE;
                i--;
            }
            break;
        case STATE_EXPONENT_VALUE:
            if (ch == 'x' || ch == 'X')
            {
                current_exponent = 1;
                if (i + 1 == len)
                {
                    coefficients[current_exponent] += current_coefficient;
                    current_coefficient = 0.0;
                    current_exponent = 0;
                }

                break;
            }

            if (ch == '+' || ch == '-')
            {
                current_exponent = 1;
                i--;
            }
            else if (ch == '^')
            {
                j = 0;
                i++;
                while (i < len && isdigit((unsigned char)formula_copy[i]))
                {
                    buf[j++] = formula_copy[i++];
                }
                buf[j] = '\0';
                current_exponent = (int)strtol(buf, NULL, 10);
                i--;
            }

            coefficients[current_exponent] += current_coefficient;
            current_coefficient = 0.0;
            current_exponent = 0;

            state = STATE_COEFFICIENT_VALUE;
            break;
        }
    }

    coefficients[current_exponent] += current_coefficient;

    Polynomial p = create_polynomial(coefficients, degree);
    free(coefficients);
    free(formula_copy);

    return p;
}

Polynomial create_polynomial(const double *coefficients, int degree)
{
    Polynomial p;
    p.degree = degree;

    // Allocate and copy coefficients
    p.coefficients = malloc((degree + 1) * sizeof(double));
    if (!p.coefficients)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= degree; i++)
        p.coefficients[i] = coefficients[i];

    init_root_array_list(&p.roots);

    int formula_lenght = 256;

    p.formula = malloc(formula_lenght * sizeof(char));
    if (!p.formula)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    polynomial_to_string(&p, p.formula, formula_lenght);

    init_point_array_list(&p.extreme_points);

    init_interval_array_list(&p.positive_value_intervals);
    init_interval_array_list(&p.negative_value_intervals);

    init_interval_array_list(&p.monotonic_increasing_intervals);
    init_interval_array_list(&p.monotonic_decreasing_intervals);

    init_point_array_list(&p.inflection_points);

    init_interval_array_list(&p.concave_intervals);
    init_interval_array_list(&p.convex_intervals);

    return p;
}

Polynomial copy_polynomial(const Polynomial *p)
{
    Polynomial copy = create_polynomial(p->coefficients, p->degree);

    return copy;
}

Polynomial polynomial_derivative(const Polynomial *p)
{

    // Handle constant polynomial
    if (p->degree <= 0)
    {
        double zero = 0.0;
        return create_polynomial(&zero, 0);
    }

    int degree = p->degree - 1;
    double *coefficients = malloc((degree + 1) * sizeof(double));

    if (!coefficients)
    {
        perror("calloc failed");
        exit(EXIT_FAILURE);
    }

    // Compute derivative
    for (int i = 1; i <= p->degree; i++)
    {
        coefficients[i - 1] = p->coefficients[i] * i;
    }

    Polynomial result = create_polynomial(coefficients, degree);
    free(coefficients);

    return result;
}

void free_polynomial(Polynomial *p)
{
    free(p->coefficients);
    p->coefficients = NULL;
    p->degree = 0;

    free(p->formula);
    p->formula = NULL;

    free_root_array_list(&p->roots);

    free_point_array_list(&p->extreme_points);

    free_interval_array_list(&p->positive_value_intervals);
    free_interval_array_list(&p->negative_value_intervals);

    free_interval_array_list(&p->monotonic_increasing_intervals);
    free_interval_array_list(&p->monotonic_decreasing_intervals);

    free_point_array_list(&p->inflection_points);

    free_interval_array_list(&p->concave_intervals);
    free_interval_array_list(&p->convex_intervals);
}
