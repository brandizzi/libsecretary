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
#include <secretary/test/project.h>
#include <stdlib.h>

static void test_project_create(CuTest *test) {
    SctProject *project = sct_project_new("libsecretary");
    CuAssertStrEquals(test, "libsecretary", sct_project_get_name(project));

    sct_project_free(project);
}

static void test_project_alter_name(CuTest *test) {
    SctProject *project = sct_project_new("libsecretary");
    CuAssertStrEquals(test, "libsecretary", sct_project_get_name(project));

    sct_project_set_name(project, "chocrotary");
    CuAssertStrEquals(test, "chocrotary", sct_project_get_name(project));
    
    sct_project_free(project);
}

static void test_project_remove_task(CuTest *test) {
    SctProject *project = sct_project_new("libsecretary");
    SctTask *task1 = sct_task_new("Create 1st task"),
         *task2 = sct_task_new("Create 2nd task"),
         *task3 = sct_task_new("Create 3rd task");

    sct_project_add_task(project, task1);
    sct_project_add_task(project, task2);
    sct_project_add_task(project, task3);

    CuAssertIntEquals(test, 3, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task1, sct_project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, task2, sct_project_get_nth_task(project, 1, false));
    CuAssertPtrEquals(test, task3, sct_project_get_nth_task(project, 2, false));

    sct_project_remove_task(project, task2);

    CuAssertIntEquals(test, 2, sct_project_count_tasks(project, false));
    CuAssertPtrEquals(test, task1, sct_project_get_nth_task(project, 0, false));
    CuAssertPtrEquals(test, task3, sct_project_get_nth_task(project, 1, false));

    sct_task_free(task1);
    sct_task_free(task2);
    sct_task_free(task3);
    sct_project_free(project);
}

void test_project_archived_tasks(CuTest *test) {
    SctProject *project = sct_project_new("libsecretary");
    SctTask *task1 = sct_task_new("Create first task"),
         *task2 = sct_task_new("Create snd task"),
         *task3 = sct_task_new("Create thrid task");

    sct_project_add_task(project, task1);
    sct_project_add_task(project, task2);
    sct_project_add_task(project, task3);

    sct_task_mark_as_done(task1);
    sct_task_mark_as_done(task2);
    sct_task_archive(task2);

    CuAssertIntEquals(test, 2, sct_project_count_tasks(project, false));
    SctTask *task = sct_project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_project_get_nth_task(project, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1,sct_project_count_tasks(project, true));
    task = sct_project_get_nth_task(project, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    sct_project_free(project);
}

void test_project_archive_tasks(CuTest *test) {
    SctProject *project = sct_project_new("libsecretary");
    SctTask *task1 = sct_task_new("Create first task"),
         *task2 = sct_task_new("Create snd task"),
         *task3 = sct_task_new("Create thrid task");

    sct_project_add_task(project, task1);
    sct_project_add_task(project, task2);
    sct_project_add_task(project, task3);

    sct_task_mark_as_done(task2);

    CuAssertIntEquals(test, 3, sct_project_count_tasks(project, false));
    SctTask *task = sct_project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_project_get_nth_task(project, 1, false);
    CuAssertPtrEquals(test, task, task2);
    task = sct_project_get_nth_task(project, 2, false);
    CuAssertPtrEquals(test, task, task3);

    sct_project_archive_tasks(project);
    
    CuAssertIntEquals(test, 2, sct_project_count_tasks(project, false));
    task = sct_project_get_nth_task(project, 0, false);
    CuAssertPtrEquals(test, task, task1);
    task = sct_project_get_nth_task(project, 1, false);
    CuAssertPtrEquals(test, task, task3);

    CuAssertIntEquals(test, 1,sct_project_count_tasks(project, true));
    task = sct_project_get_nth_task(project, 0, true);
    CuAssertPtrEquals(test, task, task2);
    
    sct_project_free(project);
}


void test_project_archived(CuTest *test) {
    SctProject *project = sct_project_new("libsecretary");

    CuAssertTrue(test, ! sct_project_is_archived(project));
    sct_project_archive(project);
    CuAssertTrue(test, sct_project_is_archived(project));
    sct_project_unarchive(project);
    CuAssertTrue(test, ! sct_project_is_archived(project));

    sct_project_free(project);
}

void test_project_archival_also_apply_for_tasks(CuTest *test) {
    SctTask *task1 = sct_task_new("Create first task"),
         *task2 = sct_task_new("Create snd task"),
         *task3 = sct_task_new("Create thrid task");

    SctProject *project1 = sct_project_new("fst proj"),
        *project2 = sct_project_new("snd proj");

    sct_project_add_task(project1, task1);
    sct_project_add_task(project2, task2);
    sct_project_add_task(project1, task3);

    sct_project_archive(project1);
    CuAssertTrue(test, sct_task_is_archived(task1));
    CuAssertTrue(test, ! sct_task_is_archived(task2));
    CuAssertTrue(test, sct_task_is_archived(task3));

    sct_project_unarchive(project1);
    CuAssertTrue(test, ! sct_task_is_archived(task1));
    CuAssertTrue(test, ! sct_task_is_archived(task2));
    CuAssertTrue(test, ! sct_task_is_archived(task3));

    sct_task_free(task1);
    sct_task_free(task2);
    sct_task_free(task3);

    sct_project_free(project1);
    sct_project_free(project2);
}


static void test_project_free_tasks(CuTest *test) {
    SctProject *project = sct_project_new("libsecretary");
    SctTask *task1 = sct_task_new("Create first task"),
         *task2 = sct_task_new("Create snd task"),
         *task3 = sct_task_new("Create thrid task");

    sct_project_add_task(project, task1);
    sct_project_add_task(project, task2);
    sct_project_add_task(project, task3);

    CuAssertIntEquals(test, sct_project_count_tasks(project, false), 3);
    CuAssertPtrEquals(test, sct_task_get_project(task1), project);
    CuAssertPtrEquals(test, sct_task_get_project(task2), project);
    CuAssertPtrEquals(test, sct_task_get_project(task3), project);

    sct_project_free(project);

    CuAssertTrue(test, !sct_task_has_project(task1));
    CuAssertTrue(test, !sct_task_has_project(task2));
    CuAssertTrue(test, !sct_task_has_project(task3));

    sct_task_free(task1);
    sct_task_free(task2);
    sct_task_free(task3);
}

static void test_project_free_name(CuTest *test) {
    SctProject *project = sct_project_new("libsecretary");
    free(project->name);
    free(project);
}

CuSuite *test_project_suite() {
    CuSuite *suite  = CuSuiteNew();
    SUITE_ADD_TEST(suite, test_project_create);
    SUITE_ADD_TEST(suite, test_project_alter_name);
    SUITE_ADD_TEST(suite, test_project_free_tasks);
    SUITE_ADD_TEST(suite, test_project_free_name);
    SUITE_ADD_TEST(suite, test_project_remove_task);
    SUITE_ADD_TEST(suite, test_project_archived_tasks);
    SUITE_ADD_TEST(suite, test_project_archive_tasks);
    SUITE_ADD_TEST(suite, test_project_archived);
    SUITE_ADD_TEST(suite, test_project_archival_also_apply_for_tasks);
    return suite;
}
