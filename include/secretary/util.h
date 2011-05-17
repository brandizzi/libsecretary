#ifndef _UTIL_H
# define _UTIL_H

#include <stdlib.h>
#include <stdio.h>

typedef int (*UtilComparator)(const void *item1, const void *item2);

char *util_copy_string(const char *string);
char *util_read_string(FILE *file);
void util_write_string(FILE *file, const char *string);

#ifdef DEBUG
#  define util_log(message) printf(message)
#else
#  define util_log(message)
#endif

#endif
