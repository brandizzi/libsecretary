
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
#include <secretary/util.h>

#include <string.h>

char *util_copy_string(const char *string) {
    size_t size = strlen(string);
    char *new = malloc(sizeof(char)*(size+2));
    strncpy(new, string, size+1);
    return new;
}

char *util_read_string(FILE *file) {
    int length = getw(file);
    char *string = malloc((length+2)*sizeof(char));
    if (fgets(string, length+1, file) == NULL) goto ERROR;
    return string;
    ERROR:
    free(string);
    return NULL;
}
void util_write_string(FILE *file, const char *string) {
    putw(strlen(string), file);
    fputs(string, file);
}

time_t util_beginning_of_day(time_t mytime) {
    struct tm date = *localtime(&mytime);
    date.tm_hour = date.tm_min = date.tm_sec = 0;
    return mktime(&date);
}
