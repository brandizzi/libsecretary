/**
 * libsecretary: a C library for managing to-do lists
 * Copyright (C) 111  Adam Victor Nazareth Brandizzi <brandizzi@gmail.com>
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

#include "config.h"
#include <secretary/test/parser.h>
#include <secretary/parser.h>
#include <string.h>


static void test_parser_v1_1_saves_struct_tm(CuTest *test) {
    remove("nofile");
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test parser");
    struct tm date;
    memset(&date, 0, sizeof(date));
    date.tm_mday = 31;
    date.tm_mon = 11;
    date.tm_year = 111;
    sct_task_schedule(task, mktime(&date));

    FILE *file = fopen("nofile", "w");
    SctParserWriterFunction write = parser_get_writer(1, 1);
    write(secretary, file);
    fclose(file);
    sct_secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    getc(file); // Major version
    getc(file); // minor version
    getw(file); // project count

    int sct_task_count = getw(file);
    CuAssertIntEquals(test, 1, sct_task_count);

    int properties = getw(file);
    CuAssertTrue(test, properties & 0x2);

    sct_util_read_string(file); // Read string

    struct tm read_date;
    if (fread(&read_date, sizeof(read_date), 1, file) < 1)
        CuAssertTrue(test, false);

    CuAssertIntEquals(test, date.tm_mday, read_date.tm_mday);
    CuAssertIntEquals(test, date.tm_mon, read_date.tm_mon);
    CuAssertIntEquals(test, date.tm_year, read_date.tm_year);

    fclose(file);

    remove("nofile");

}

static void test_parser_v1_2(CuTest *test) {
    SctSecretary *secretary = sct_secretary_new();

    SctProject *project1 = sct_secretary_create_project(secretary, "libsecretary"),
            *project2 = sct_secretary_create_project(secretary, "chocrotary");

    SctTask *task1 = sct_secretary_create_task(secretary, "Test notebook"),
         *task2 = sct_secretary_create_task(secretary, "Create Cocoa interface"),
         *task3 = sct_secretary_create_task(secretary, "Buy pequi");

    time_t now = time(NULL), future_time = now+60*60*48;
    sct_secretary_schedule_task(secretary, task2, future_time);
    sct_secretary_schedule_task(secretary, task3, now);

    sct_secretary_move_task_to_project(secretary, project1, task1);
    sct_secretary_move_task_to_project(secretary, project2, task2);

    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));

    remove("nofile");
    FILE *file = fopen("nofile", "w");
    SctParserWriterFunction writer = parser_get_writer(1, 2);
    writer(secretary, file);
    fclose(file);
    sct_secretary_free(secretary);
    
    // Retrieving it
    file = fopen("nofile", "r");
    // Ignore version number
    getc(file);     getc(file);
    SctParserReaderFunction reader = parser_get_reader(1, 2);
    secretary = reader(file);

    CuAssertIntEquals(test, 3, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            sct_secretary_count_tasks_scheduled_for_today(secretary,false));
    CuAssertIntEquals(test, 2, 
            sct_secretary_count_tasks_scheduled_for(secretary, future_time, false));

    SctTask *task = sct_secretary_get_nth_done_task(secretary, 0, false);
    CuAssertTrue(test, sct_task_is_done(task));
    CuAssertStrEquals(test, "Buy pequi", sct_task_get_description(task));
    CuAssertPtrEquals(test, sct_task_get_project(task), NULL);
    CuAssertIntEquals(test, sct_util_beginning_of_day(now), sct_task_get_scheduled_date(task));

    task = sct_secretary_get_nth_done_task(secretary, 1, false);
    CuAssertStrEquals(test, "Create Cocoa interface", 
            sct_task_get_description(task));
    CuAssertTrue(test, sct_task_get_project(task) != NULL);
    CuAssertStrEquals(test, "chocrotary", 
            sct_project_get_name(sct_task_get_project(task)));
    CuAssertIntEquals(test, sct_util_beginning_of_day(future_time),
            sct_task_get_scheduled_date(task));

    task = sct_secretary_get_nth_done_task(secretary, 2, false);
    CuAssertStrEquals(test, "Test notebook", sct_task_get_description(task));
    CuAssertTrue(test, sct_task_get_project(task) != NULL);
    CuAssertStrEquals(test, "libsecretary", 
            sct_project_get_name(sct_task_get_project(task)));
    CuAssertTrue(test, ! sct_task_is_scheduled(task));

    sct_secretary_free(secretary);

    remove("nofile");

}

static void test_parser_v1_2_save_archival(CuTest *test) {
    remove("nofile");
    SctSecretary *secretary = sct_secretary_new();

    SctTask *task1 = sct_secretary_create_task(secretary, "Test parser"),
         *task2 = sct_secretary_create_task(secretary, "Create Cocoa interface"),
         *task3 = sct_secretary_create_task(secretary, "Buy pequi");

    task1 = NULL;
    sct_task_mark_as_done(task2);
    sct_secretary_archive_inbox_tasks(secretary);
    sct_task_mark_as_done(task3);

    CuAssertIntEquals(test, 2, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, true));
    CuAssertIntEquals(test, 3, sct_secretary_count_all_tasks(secretary));
    CuAssertIntEquals(test, 2, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, true));

    FILE *file = fopen("nofile", "w");
    SctParserWriterFunction writer = parser_get_writer(1, 2);
    writer(secretary, file);
    fclose(file);
    sct_secretary_free(secretary);
    
    // Retrieving it
    file = fopen("nofile", "r");
    // Ignore version number
    getc(file);     getc(file);
    SctParserReaderFunction reader = parser_get_reader(1, 2);
    secretary = reader(file);

    CuAssertIntEquals(test, 2, sct_secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_tasks(secretary, true));
    CuAssertIntEquals(test, 3, sct_secretary_count_all_tasks(secretary));

    CuAssertIntEquals(test, 2, sct_secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, sct_secretary_count_inbox_tasks(secretary, true));

    SctTask *task = sct_secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertTrue(test, !sct_task_is_done(task));
    CuAssertTrue(test, !sct_task_is_scheduled(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));
    CuAssertStrEquals(test, "Test parser", sct_task_get_description(task));


    task = sct_secretary_get_nth_inbox_task(secretary, 1, false);
    CuAssertTrue(test, sct_task_is_done(task));
    CuAssertTrue(test, !sct_task_is_archived(task));
    CuAssertTrue(test, !sct_task_is_scheduled(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));
    CuAssertStrEquals(test, "Buy pequi", sct_task_get_description(task));

    task = sct_secretary_get_nth_inbox_task(secretary, 0, true);
    CuAssertTrue(test, sct_task_is_done(task));
    CuAssertTrue(test, sct_task_is_archived(task)); // IMPORTANT!
    CuAssertTrue(test, !sct_task_is_scheduled(task));
    CuAssertPtrEquals(test, NULL, sct_task_get_project(task));
    CuAssertStrEquals(test, "Create Cocoa interface", sct_task_get_description(task));

    sct_secretary_free(secretary);

    remove("nofile");

}

static void test_parser_v1_2_saves_struct_tm(CuTest *test) {
    remove("nofile");
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test parser");
    struct tm date;
    memset(&date, 0, sizeof(date));
    date.tm_mday = 31;
    date.tm_mon = 11;
    date.tm_year = 111;
    sct_task_schedule(task, mktime(&date));
    

    FILE *file = fopen("nofile", "w");
    SctParserWriterFunction write = parser_get_writer(1, 2);
    write(secretary, file);
    fclose(file);
    sct_secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    getc(file); // Major version
    getc(file); // minor version
    getw(file); // project count

    int sct_task_count = getw(file);
    CuAssertIntEquals(test, 1, sct_task_count);

    int properties = getw(file);
    CuAssertTrue(test, properties & 0x2);

    sct_util_read_string(file); // Read string

    struct tm read_date;
    if (fread(&read_date, sizeof(read_date), 1, file) < 1)
        CuAssertTrue(test, false);

    CuAssertIntEquals(test, date.tm_mday, read_date.tm_mday);
    CuAssertIntEquals(test, date.tm_mon, read_date.tm_mon);
    CuAssertIntEquals(test, date.tm_year, read_date.tm_year);

    fclose(file);

    remove("nofile");
}

static void test_parser_v1_3_saves_task_number(CuTest *test) {
    remove("nofile");
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Test parser"),
        *task2 = sct_secretary_create_task(secretary, "Test parser with id");
    CuAssertIntEquals(test, 1, task1->number);
    CuAssertIntEquals(test, 2, task2->number);

    time_t mytime = time(NULL);
    sct_task_schedule(task1, mytime);

    FILE *file = fopen("nofile", "w");
    SctParserWriterFunction write = parser_get_writer(1, 3);
    write(secretary, file);
    fclose(file);
    sct_secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    CuAssertIntEquals(test, 1, getc(file)); // Major version
    CuAssertIntEquals(test, 3, getc(file)); // minor version
    getw(file); // project count

    int sct_task_count = getw(file);
    CuAssertIntEquals(test, 2, sct_task_count);

    /*int properties = */getw(file);
    int number = getw(file);
    CuAssertIntEquals(test, 1, number);
    time_t creation_date = sct_util_read_number(file, 8); /* Scheduled date */


    sct_util_read_string(file); // Read string

    time_t read_date = sct_util_read_number(file, 8); /* Scheduled date */

    /*int properties = */getw(file);
    number = getw(file);
    CuAssertIntEquals(test, 2, number);
    creation_date = sct_util_read_number(file, 8); /* Scheduled date */


    sct_util_read_string(file); // Read string

    fclose(file);

    remove("nofile");
}

static void test_parser_v1_3_saves_creation_date(CuTest *test) {
    remove("nofile");
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task1 = sct_secretary_create_task(secretary, "Test parser"),
        *task2 = sct_secretary_create_task(secretary, "Test parser");
    task2->created_at += SCT_UTIL_SECONDS_IN_DAY*3;
    time_t time1 = sct_task_get_creation_date(task1),
        time2 = sct_task_get_creation_date(task2);
    sct_task_schedule(task1, time(NULL));

    FILE *file = fopen("nofile", "w");
    SctParserWriterFunction write = parser_get_writer(1, 3);
    write(secretary, file);
    fclose(file);
    sct_secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    getc(file); // Major version
    getc(file); // minor version
    getw(file); // project count

    int sct_task_count = getw(file);
    CuAssertIntEquals(test, 2, sct_task_count);

    /*int properties = */getw(file);
    /*int number = */getw(file);
    time_t creation_date = sct_util_read_number(file, 8); // Read time
    CuAssertIntEquals(test, time1, creation_date);

    sct_util_read_string(file); // Read string

    time_t read_date = sct_util_read_number(file, 8); // Read time


    /*int properties = */getw(file);
    /*int number = */getw(file);
    creation_date = sct_util_read_number(file, 8); // Read time
    CuAssertIntEquals(test, time2, creation_date);

    fclose(file);

    remove("nofile");
}

static void test_parser_v1_3_saves_time_t(CuTest *test) {
    remove("nofile");
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test parser");
    struct tm date;
    time_t mytime = time(NULL);
    sct_task_schedule(task, mytime);

    FILE *file = fopen("nofile", "w");
    SctParserWriterFunction write = parser_get_writer(1, 3);
    write(secretary, file);
    fclose(file);
    sct_secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    getc(file); // Major version
    getc(file); // minor version
    getw(file); // project count

    int sct_task_count = getw(file);
    CuAssertIntEquals(test, 1, sct_task_count);

    int properties = getw(file);
    CuAssertTrue(test, properties & 0x2);
    /*int number = */getw(file);
    sct_util_read_string(file); // Read string

    fclose(file);

    remove("nofile");
}

static void test_parser_v1_3_read(CuTest *test) {
    remove("nofile");
    SctSecretary *secretary = sct_secretary_new();
    SctTask *task = sct_secretary_create_task(secretary, "Test parser");
    time_t now = task->created_at += SCT_UTIL_SECONDS_IN_DAY*12;
    time_t mytime = time(NULL);
    sct_task_schedule(task, mytime);

    FILE *file = fopen("nofile", "w");
    SctParserWriterFunction write = parser_get_writer(1, 3);
    write(secretary, file);
    fclose(file);
    sct_secretary_free(secretary);

    // See what is saved.
    SctParserReaderFunction read = parser_get_reader(1, 3);
    file = fopen("nofile", "r");
    CuAssertIntEquals(test, 1, getc(file)); // Major version
    CuAssertIntEquals(test, 3, getc(file)); // minor version

    secretary = read(file);
    CuAssertIntEquals(test, 1, sct_secretary_count_all_tasks(secretary));

    task = sct_secretary_get_nth_task(secretary, 0);
    CuAssertTrue(test, task != NULL);
    CuAssertIntEquals(test, 1, task->number);
    CuAssertIntEquals(test, now, sct_task_get_creation_date(task));
    CuAssertIntEquals(test,
            sct_util_beginning_of_day(mytime), sct_task_get_scheduled_date(task));
    
    fclose(file);

    remove("nofile");
}

CuSuite *test_parser_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_parser_v1_1_saves_struct_tm);
    SUITE_ADD_TEST(suite, test_parser_v1_2);
    SUITE_ADD_TEST(suite, test_parser_v1_2_save_archival);
    SUITE_ADD_TEST(suite, test_parser_v1_2_saves_struct_tm);
    SUITE_ADD_TEST(suite, test_parser_v1_3_saves_time_t);
    SUITE_ADD_TEST(suite, test_parser_v1_3_saves_task_number);
    SUITE_ADD_TEST(suite, test_parser_v1_3_saves_creation_date);
    SUITE_ADD_TEST(suite, test_parser_v1_3_read);
    return suite;
}
