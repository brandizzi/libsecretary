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
/* This is auto-generated code. Edit at your own peril. */
#include <stdio.h>
#include <stdlib.h>

#include <secretary/test/secretary.h>
#include <secretary/test/notebook.h>
#include <secretary/test/task.h>
#include <secretary/test/project.h>
#include <secretary/test/util.h>
#include <secretary/test/list.h>
#include <secretary/test/pubsub.h>
#include <secretary/test/parser.h>
#include <secretary/test/optimization_requisites.h>

#include <CuTest.h>


int RunAllTests(void) 
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, test_project_suite());
    CuSuiteAddSuite(suite, test_task_suite());
    CuSuiteAddSuite(suite, test_secretary_suite());
    CuSuiteAddSuite(suite, test_notebook_suite());    
    CuSuiteAddSuite(suite, test_list_suite());
    CuSuiteAddSuite(suite, test_pubsub_suite());
    CuSuiteAddSuite(suite, test_util_suite());
    CuSuiteAddSuite(suite, test_parser_suite());
    CuSuiteAddSuite(suite, test_optimization_requisites_suite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    CuStringDelete(output);
    CuSuiteDelete(suite);
    return suite->failCount;
}

int main(void)
{
    return RunAllTests();
}

