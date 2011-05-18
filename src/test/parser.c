#include <secretary/test/parser.h>
#include <secretary/parser.h>

static void test_parser_v1_2(CuTest *test) {
    Secretary *secretary = secretary_new();

    Project *project1 = secretary_create_project(secretary, "libsecretary"),
            *project2 = secretary_create_project(secretary, "chocrotary");

    Task *task1 = secretary_create_task(secretary, "Test notebook"),
         *task2 = secretary_create_task(secretary, "Create Cocoa interface"),
         *task3 = secretary_create_task(secretary, "Buy pequi");

    time_t now = time(NULL), future_time = now+60*60*48;
    struct tm date = *localtime(&future_time);
    task_schedule(task2, date);
    task_schedule(task3, *localtime(&now));

    project_add_task(project1, task1);
    project_add_task(project2, task2);

    task_mark_as_done(task1);
    task_mark_as_done(task2);
    task_mark_as_done(task3);

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
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

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
    CuAssertIntEquals(test, 0, secretary_count_inbox_tasks(secretary, false));
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled(secretary, false));
    CuAssertIntEquals(test, 1, 
            secretary_count_tasks_scheduled_for_today(secretary,false));
    CuAssertIntEquals(test, 2, 
            secretary_count_tasks_scheduled_for(secretary, date, false));

    Task *task = secretary_get_nth_done_task(secretary, 1, false);
    CuAssertTrue(test, task_is_done(task));
    CuAssertTrue(test, task_get_project(task) != NULL);
    CuAssertStrEquals(test, "chocrotary", 
            project_get_name(task_get_project(task)));
    CuAssertIntEquals(test, date.tm_mday, task_get_scheduled_date(task).tm_mday);
    CuAssertIntEquals(test, date.tm_mday, task_get_scheduled_date(task).tm_mday);

    task = secretary_get_nth_done_task(secretary, 2, false);
    CuAssertStrEquals(test, "Buy pequi", task_get_description(task));
    CuAssertPtrEquals(test, NULL, task_get_project(task));

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

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
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

    CuAssertIntEquals(test, 3, secretary_count_tasks(secretary));
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


CuSuite *test_parser_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_parser_v1_2);
    SUITE_ADD_TEST(suite, test_parser_v1_2_save_archival);
    return suite;
}
