#ifndef EXTENDED_VALUE_H
#define EXTENDED_VALUE_H

typedef enum
{
    VALUE_FINITE,
    VALUE_POS_INF,
    VALUE_NEG_INF,
    VALUE_UNDEFINED
} ValueType;

typedef struct
{
    ValueType type;
    double value;
} ExtendedValue;

// creation
ExtendedValue extended_value_finite(double value);
ExtendedValue extended_value_pos_infinity(void);
ExtendedValue extended_value_neg_infinity(void);
ExtendedValue extended_value_undefined(void);

// utilities
void extended_value_to_string(const ExtendedValue *ev,
                              char *buffer,
                              size_t buffer_size);

int compare_extended_values(const ExtendedValue *a, const ExtendedValue *b);

#endif // EXTENDED_VALUE_H
