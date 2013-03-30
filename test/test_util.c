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
#include <secretary/test/util.h>
#include <secretary/util.h>
#include <stdlib.h>

static void test_util_beginning_of_day(CuTest *test) {
    time_t now = time(NULL), stripped = sct_util_beginning_of_day(now);
    struct tm tm_now = *localtime(&now), tm_stripped = *localtime(&stripped);

    CuAssertIntEquals(test, tm_now.tm_mon, tm_stripped.tm_mon);
    CuAssertIntEquals(test, tm_now.tm_year, tm_stripped.tm_year);
    CuAssertIntEquals(test, tm_now.tm_mday, tm_stripped.tm_mday);
    CuAssertIntEquals(test, 0, tm_stripped.tm_hour);
    CuAssertIntEquals(test, 0, tm_stripped.tm_min);
    CuAssertIntEquals(test, 0, tm_stripped.tm_sec);
}

static void test_util_beginning_of_day_verify_if_scheduled(CuTest *test) {
    time_t now = time(NULL), stripped = sct_util_beginning_of_day(now);
    struct tm tm_now = *localtime(&now), tm_stripped = *localtime(&stripped);

    CuAssertIntEquals(test, tm_now.tm_mon, tm_stripped.tm_mon);
    CuAssertIntEquals(test, tm_now.tm_year, tm_stripped.tm_year);
    CuAssertIntEquals(test, tm_now.tm_mday, tm_stripped.tm_mday);
    CuAssertIntEquals(test, 0, tm_stripped.tm_hour);
    CuAssertIntEquals(test, 0, tm_stripped.tm_min);
    CuAssertIntEquals(test, 0, tm_stripped.tm_sec);
}

static void test_util_read_write_number(CuTest *test) {
    uint_least64_t value = 0x1122334455667788;
    FILE *file = fopen("myfile", "w");
    sct_util_write_number(file, value, 8);
    
    fclose(file);
    file = fopen("myfile", "r");
    
    value = sct_util_read_number(file, 8);
    CuAssertIntEquals(test, 0x1122334455667788, value);
    
    fclose(file);
    file = fopen("myfile", "w");
    
    sct_util_write_number(file, value, 4);    

    fclose(file);
    file = fopen("myfile", "r");
    
    value = sct_util_read_number(file, 4);
    CuAssertIntEquals(test, value, 0x55667788);
    remove("myfile");
}

CuSuite *test_util_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_util_beginning_of_day);
    SUITE_ADD_TEST(suite, test_util_beginning_of_day_verify_if_scheduled);
    SUITE_ADD_TEST(suite, test_util_read_write_number);
    return suite;
}
