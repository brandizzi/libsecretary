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
#include <secretary/test/parser.h>
#include <secretary/parser.h>

static void test_parser_v1_1_saves_struct_tm(CuTest *test) {
    remove("nofile");
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test parser");
    struct tm date;
    date.tm_mday = 31;
    date.tm_mon = 12;
    date.tm_year = 2011;
    task_schedule(task, timegm(&date));

    FILE *file = fopen("nofile", "w");
    ParserWriterFunction write = parser_get_writer(1, 1);
    write(secretary, file);
    fclose(file);
    secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    getc(file); // Major version
    getc(file); // minor version
    getw(file); // project count

    int task_count = getw(file);
    CuAssertIntEquals(test, 1, task_count);

    int properties = getw(file);
    CuAssertTrue(test, properties & 0x2);

    util_read_string(file); // Read string

    struct tm read_date;
    fread(&read_date, sizeof(read_date), 1, file);

    CuAssertIntEquals(test, date.tm_mday, read_date.tm_mday);
    CuAssertIntEquals(test, date.tm_mon, read_date.tm_mon);
    CuAssertIntEquals(test, date.tm_year, read_date.tm_year);

    fclose(file);

    remove("nofile");

}

static void test_parser_v1_2(CuTest *test) {
    Secretary *secretary = secretary_new();

    Project *project1 = secretary_create_project(secretary, "libsecretary"),
            *project2 = secretary_create_project(secretary, "chocrotary");

    Task *task1 = secretary_create_task(secretary, "Test notebook"),
         *task2 = secretary_create_task(secretary, "Create Cocoa interface"),
         *task3 = secretary_create_task(secretary, "Buy pequi");

    time_t now = time(NULL), future_time = now+60*60*48;
    task_schedule(task2, future_time);
    task_schedule(task3, now);

    secretary_move_task_to_project(secretary, project1, task1);
    secretary_move_task_to_project(secretary, project2, task2);

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 2, secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));

    remove("nofile");
    FILE *file = fopen("nofile", "w");
    ParserWriterFunction writer = parser_get_writer(1, 2);
    writer(secretary, file);
    fclose(file);
    secretary_free(secretary);
    
    // Retrieving it
    file = fopen("nofile", "r");
    // Ignore version number
    getc(file);     getc(file);
    ParserReaderFunction reader = parser_get_reader(1, 2);
    secretary = reader(file);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled_for_today(secretary,false));
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled_for(secretary, future_time, false));

    Task *task = secretary_get_nth_done_task(secretary, 0, false);
    CuAssertTrue(test, task_is_done(task));
    CuAssertStrEquals(test, "Buy pequi", task_get_description(task));
    CuAssertPtrEquals(test, task_get_project(task), NULL);
    CuAssertIntEquals(test, util_beginning_of_day(now), task_get_scheduled_date(task));

    task = secretary_get_nth_done_task(secretary, 1, false);
    CuAssertStrEquals(test, "Create Cocoa interface", 
            task_get_description(task));
    CuAssertTrue(test, task_get_project(task) != NULL);
    CuAssertStrEquals(test, "chocrotary", 
            project_get_name(task_get_project(task)));
    CuAssertIntEquals(test, util_beginning_of_day(future_time),
            task_get_scheduled_date(task));

    task = secretary_get_nth_done_task(secretary, 2, false);
    CuAssertStrEquals(test, "Test notebook", task_get_description(task));
    CuAssertTrue(test, task_get_project(task) != NULL);
    CuAssertStrEquals(test, "libsecretary", 
            project_get_name(task_get_project(task)));
    CuAssertTrue(test, ! task_is_scheduled(task));

    secretary_free(secretary);

    remove("nofile");

}

static void test_parser_v1_2_save_archival(CuTest *test) {
    remove("nofile");
    Secretary *secretary = secretary_new();

    Task *task1 = secretary_create_task(secretary, "Test parser"),
         *task2 = secretary_create_task(secretary, "Create Cocoa interface"),
         *task3 = secretary_create_task(secretary, "Buy pequi");

    task1 = NULL;
    task_mark_as_done(task2);
    secretary_archive_inbox_tasks(secretary);
    task_mark_as_done(task3);

    CuAssertIntEquals(test, 2, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, true));
    CuAssertIntEquals(test, 3, secretary_count_all_tasks(secretary));
    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, true));

    FILE *file = fopen("nofile", "w");
    ParserWriterFunction writer = parser_get_writer(1, 2);
    writer(secretary, file);
    fclose(file);
    secretary_free(secretary);
    
    // Retrieving it
    file = fopen("nofile", "r");
    // Ignore version number
    getc(file);     getc(file);
    ParserReaderFunction reader = parser_get_reader(1, 2);
    secretary = reader(file);

    CuAssertIntEquals(test, 2, secretary_count_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_tasks(secretary, true));
    CuAssertIntEquals(test, 3, secretary_count_all_tasks(secretary));

    CuAssertIntEquals(test, 2, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 1, secretary_count_inbox_tasks(secretary, true));

    Task *task = secretary_get_nth_inbox_task(secretary, 0, false);
    CuAssertTrue(test, !task_is_done(task));
    CuAssertTrue(test, !task_is_scheduled(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));
    CuAssertStrEquals(test, "Test parser", task_get_description(task));


    task = secretary_get_nth_inbox_task(secretary, 1, false);
    CuAssertTrue(test, task_is_done(task));
    CuAssertTrue(test, !task_is_archived(task));
    CuAssertTrue(test, !task_is_scheduled(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));
    CuAssertStrEquals(test, "Buy pequi", task_get_description(task));

    task = secretary_get_nth_inbox_task(secretary, 0, true);
    CuAssertTrue(test, task_is_done(task));
    CuAssertTrue(test, task_is_archived(task)); // IMPORTANT!
    CuAssertTrue(test, !task_is_scheduled(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));
    CuAssertStrEquals(test, "Create Cocoa interface", task_get_description(task));

    secretary_free(secretary);

    remove("nofile");

}

static void test_parser_v1_2_saves_struct_tm(CuTest *test) {
    remove("nofile");
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test parser");
    struct tm date;
    date.tm_mday = 31;
    date.tm_mon = 12;
    date.tm_year = 2011;
    task_schedule(task, timegm(&date));

    FILE *file = fopen("nofile", "w");
    ParserWriterFunction write = parser_get_writer(1, 2);
    write(secretary, file);
    fclose(file);
    secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    getc(file); // Major version
    getc(file); // minor version
    getw(file); // project count

    int task_count = getw(file);
    CuAssertIntEquals(test, 1, task_count);

    int properties = getw(file);
    CuAssertTrue(test, properties & 0x2);

    util_read_string(file); // Read string

    struct tm read_date;
    fread(&read_date, sizeof(read_date), 1, file);

    CuAssertIntEquals(test, date.tm_mday, read_date.tm_mday);
    CuAssertIntEquals(test, date.tm_mon, read_date.tm_mon);
    CuAssertIntEquals(test, date.tm_year, read_date.tm_year);

    fclose(file);

    remove("nofile");
}

static void test_parser_v1_3_saves_task_number(CuTest *test) {
    remove("nofile");
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Test parser"),
        *task2 = secretary_create_task(secretary, "Test parser with id");
    CuAssertIntEquals(test, 1, task1->number);
    CuAssertIntEquals(test, 2, task2->number);

    time_t mytime = time(NULL);
    task_schedule(task1, mytime);

    FILE *file = fopen("nofile", "w");
    ParserWriterFunction write = parser_get_writer(1, 3);
    write(secretary, file);
    fclose(file);
    secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    CuAssertIntEquals(test, 1, getc(file)); // Major version
    CuAssertIntEquals(test, 3, getc(file)); // minor version
    getw(file); // project count

    int task_count = getw(file);
    CuAssertIntEquals(test, 2, task_count);

    /*int properties = */getw(file);
    int number = getw(file);
    CuAssertIntEquals(test, 1, number);
    time_t creation_date; /* Scheduled date */
    fread(&creation_date, sizeof(time_t), 1, file);


    util_read_string(file); // Read string

    time_t read_date; /* Scheduled date */
    fread(&read_date, sizeof(time_t), 1, file);

    /*int properties = */getw(file);
    number = getw(file);
    CuAssertIntEquals(test, 2, number);
    creation_date; /* Scheduled date */
    fread(&creation_date, sizeof(time_t), 1, file);


    util_read_string(file); // Read string

    fclose(file);

    remove("nofile");
}

static void test_parser_v1_3_saves_creation_date(CuTest *test) {
    remove("nofile");
    Secretary *secretary = secretary_new();
    Task *task1 = secretary_create_task(secretary, "Test parser"),
        *task2 = secretary_create_task(secretary, "Test parser");
    task2->created_at += UTIL_SECONDS_IN_DAY*3;
    time_t time1 = task_get_creation_date(task1),
        time2 = task_get_creation_date(task2);
    task_schedule(task1, time(NULL));

    FILE *file = fopen("nofile", "w");
    ParserWriterFunction write = parser_get_writer(1, 3);
    write(secretary, file);
    fclose(file);
    secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    getc(file); // Major version
    getc(file); // minor version
    getw(file); // project count

    int task_count = getw(file);
    CuAssertIntEquals(test, 2, task_count);

    /*int properties = */getw(file);
    /*int number = */getw(file);
    time_t creation_date; // Read time
    fread(&creation_date, sizeof(time_t), 1, file);
    CuAssertIntEquals(test, time1, creation_date);

    util_read_string(file); // Read string

    time_t read_date; // Read time
    fread(&read_date, sizeof(time_t), 1, file);


    /*int properties = */getw(file);
    /*int number = */getw(file);
    creation_date; // Read time
    fread(&creation_date, sizeof(time_t), 1, file);
    CuAssertIntEquals(test, time2, creation_date);

    fclose(file);

    remove("nofile");
}

static void test_parser_v1_3_saves_time_t(CuTest *test) {
    remove("nofile");
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test parser");
    struct tm date;
    time_t mytime = time(NULL);
    task_schedule(task, mytime);

    FILE *file = fopen("nofile", "w");
    ParserWriterFunction write = parser_get_writer(1, 3);
    write(secretary, file);
    fclose(file);
    secretary_free(secretary);

    // See what is saved.
    file = fopen("nofile", "r");
    getc(file); // Major version
    getc(file); // minor version
    getw(file); // project count

    int task_count = getw(file);
    CuAssertIntEquals(test, 1, task_count);

    int properties = getw(file);
    CuAssertTrue(test, properties & 0x2);
    /*int number = */getw(file);
    util_read_string(file); // Read string

    fclose(file);

    remove("nofile");
}

static void test_parser_v1_3_read(CuTest *test) {
    remove("nofile");
    Secretary *secretary = secretary_new();
    Task *task = secretary_create_task(secretary, "Test parser");
    time_t now = task->created_at += UTIL_SECONDS_IN_DAY*12;
    time_t mytime = time(NULL);
    task_schedule(task, mytime);

    FILE *file = fopen("nofile", "w");
    ParserWriterFunction write = parser_get_writer(1, 3);
    write(secretary, file);
    fclose(file);
    secretary_free(secretary);

    // See what is saved.
    ParserReaderFunction read = parser_get_reader(1, 3);
    file = fopen("nofile", "r");
    CuAssertIntEquals(test, 1, getc(file)); // Major version
    CuAssertIntEquals(test, 3, getc(file)); // minor version

    secretary = read(file);
    CuAssertIntEquals(test, 1, secretary_count_all_tasks(secretary));

    task = secretary_get_nth_task(secretary, 0);
    CuAssertTrue(test, task != NULL);
    CuAssertIntEquals(test, 1, task->number);
    CuAssertIntEquals(test, now, task_get_creation_date(task));
    CuAssertIntEquals(test,
            util_beginning_of_day(mytime), task_get_scheduled_date(task));
    
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
