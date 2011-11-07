
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
#include <secretary/test/task.h>
#include <secretary/project.h>
#include <stdlib.h>

static void test_task_create(CuTest *test) {
    Task *task = task_new("Do something" );
    CuAssertStrEquals(test, "Do something", task_get_description(task));

    task_free(task);
}

static void test_task_alter_description(CuTest *test) {
    Task *task = task_new("Do something" );
    CuAssertStrEquals(test, "Do something", task_get_description(task));
    task_set_description(task, "Do something else");
    CuAssertStrEquals(test, "Do something else", task_get_description(task));
    
    task_free(task);
}

static void test_task_remove_from_project(CuTest *test) {
    Task *task = task_new("Do something" );
    Project *project = project_new("libsecretary");
    CuAssertPtrEquals(test, task_get_project(task), NULL);

    task_set_project(task, project);
    CuAssertPtrEquals(test, task_get_project(task), project);
    task_unset_project(task);
    CuAssertPtrEquals(test, task_get_project(task), NULL);

    task_free(task);
    project_free(project);
}

static void test_task_is_in_project(CuTest *test) {
    Task *task = task_new("Do something" );
    Project *project = project_new("libsecretary");
    CuAssertTrue(test, !task_is_in_project(task, project));

    task_set_project(task, project);
    CuAssertTrue(test, task_is_in_project(task, project));

    task_free(task);
    project_free(project);
}

static void test_task_switch_done_status(CuTest *test) {
    Task *task = task_new("Do something" );
    CuAssertTrue(test, !task_is_done(task));
    task_mark_as_done(task);
    CuAssertTrue(test, task_is_done(task));
    task_unmark_as_done(task);
    CuAssertTrue(test, !task_is_done(task));
    task_switch_done_status(task);
    CuAssertTrue(test, task_is_done(task));
    task_switch_done_status(task);
    CuAssertTrue(test, !task_is_done(task));

    task_free(task);
}

static void test_task_is_in_inbox(CuTest *test) {
    Task *task = task_new("Do something" );
    CuAssertTrue(test, (task_is_in_inbox(task) && !task_is_archived(task)));

    task_mark_as_done(task);
    CuAssertTrue(test, (task_is_in_inbox(task) && !task_is_archived(task)));

    task_schedule(task, time(NULL));
    CuAssertTrue(test, !(task_is_in_inbox(task) && !task_is_archived(task)));
    task_unschedule(task);
    CuAssertTrue(test, (task_is_in_inbox(task) && !task_is_archived(task)));

    Project *project = project_new("libsecretary");
    project_add_task(project, task);
    CuAssertTrue(test, !(task_is_in_inbox(task) && !task_is_archived(task)));
    project_remove_task(project, task);
    CuAssertTrue(test, (task_is_in_inbox(task) && !task_is_archived(task)));

    task_archive(task);
    CuAssertTrue(test, !(task_is_in_inbox(task) && !task_is_archived(task)));

    project_free(project);
    task_free(task);
}

static void test_task_archived_is_in_inbox(CuTest *test) {
    Task *task = task_new("Do something" );
    CuAssertTrue(test, (task_is_in_inbox(task) && !task_is_archived(task)));

    CuAssertTrue(test, !(task_is_in_inbox(task) && task_is_archived(task)));
    task_mark_as_done(task);
    CuAssertTrue(test, !(task_is_in_inbox(task) && task_is_archived(task)));
    task_archive(task);
    CuAssertTrue(test, (task_is_in_inbox(task) && task_is_archived(task)));

    task_schedule(task, time(NULL));
    CuAssertTrue(test, !(task_is_in_inbox(task) && task_is_archived(task)));
    task_unschedule(task);
    CuAssertTrue(test, (task_is_in_inbox(task) && task_is_archived(task)));

    Project *project = project_new("libsecretary");
    project_add_task(project, task);
    CuAssertTrue(test, !(task_is_in_inbox(task) && task_is_archived(task)));
    project_remove_task(project, task);
    CuAssertTrue(test, (task_is_in_inbox(task) && task_is_archived(task)));

    project_free(project);
    task_free(task);
}

static void test_task_archive(CuTest *test) {
    Task *task = task_new("Do something" );
    CuAssertTrue(test, !task_is_archived(task));
    task_archive(task); // does nothing, task is not done
    CuAssertTrue(test, !task_is_archived(task));
    task_mark_as_done(task);
    task_archive(task); // Now it archives
    CuAssertTrue(test, task_is_archived(task));

    // Now testing unarchival - which is done by undoing the task
    task_unmark_as_done(task);
    CuAssertTrue(test, !task_is_archived(task));
    task_mark_as_done(task); // Should not return to archives
    CuAssertTrue(test, !task_is_archived(task));
    
    task_free(task);
}

static void test_task_created_at(CuTest *test) {
    Task *task = task_new("Do something" );
    time_t now = time(NULL);
    CuAssertTrue(test, now-task_get_creation_date(task) < 2); // Less than 2 seconds

    task_free(task);
}

CuSuite *test_task_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_task_create);
    SUITE_ADD_TEST(suite, test_task_alter_description);
    SUITE_ADD_TEST(suite, test_task_remove_from_project);
    SUITE_ADD_TEST(suite, test_task_is_in_project);
    SUITE_ADD_TEST(suite, test_task_switch_done_status);
    SUITE_ADD_TEST(suite, test_task_archive);
    SUITE_ADD_TEST(suite, test_task_is_in_inbox);
    SUITE_ADD_TEST(suite, test_task_archived_is_in_inbox);
    SUITE_ADD_TEST(suite, test_task_created_at);
    return suite;
}
