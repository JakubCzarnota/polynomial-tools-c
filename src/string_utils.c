#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "string_utils.h"

#include <ctype.h>

void remove_whitespace(char *str)
{
    int i = 0, j = 0;

    while (str[i] != '\0') {
        if (!isspace((unsigned char)str[i])) {
            str[j++] = str[i];
        }
        i++;
    }

    str[j] = '\0';
}


char *copy_string(const char *src)
{
    char *dst = malloc(strlen(src) + 1);
    if (dst)
    {
        strcpy(dst, src);
    }
    return dst;
}