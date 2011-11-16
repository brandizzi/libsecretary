
/**
 * libsecretary: a C library for managing to-do lists
 * Copyright (C) 2011  Adam Victor Nazareth Brandizzi <brandizzi@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * You can get the latest version of this file at 
 * http://bitbucket.org/brandizzi/libsecretary/
 */
#ifndef _UTIL_H
# define _UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define UTIL_SECONDS_IN_DAY (60*60*24)

typedef int (*UtilComparator)(const void *item1, const void *item2);

char *util_copy_string(const char *string);
char *util_read_string(FILE *file);
void util_write_string(FILE *file, const char *string);
time_t util_beginning_of_day(time_t time);

#ifdef DEBUG
#  define util_log(message) printf(message)
#else
#  define util_log(message)
#endif

#endif
