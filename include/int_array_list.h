#ifndef DOUBLE_ARRAYLIST
#define DOUBLE_ARRAYLIST

typedef struct
{
    int *values;
    int size;
    int capacity;
} IntArrayList;

void init_int_array_list(IntArrayList *list);

void int_array_list_to_string(const IntArrayList *list, char *buffer, int buffer_size);

void int_array_list_add_value(IntArrayList *list, int value);

void int_array_list_remove_last_value(IntArrayList *list);

void sort_int_array_list(IntArrayList *list);

void free_int_array_list(IntArrayList *list);

#endif // DOUBLE_ARRAYLIST
