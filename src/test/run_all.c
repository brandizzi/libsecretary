/* This is auto-generated code. Edit at your own peril. */
#include <stdio.h>
#include <stdlib.h>

#include <secretary/test/secretary.h>
#include <secretary/test/notebook.h>
#include <secretary/test/task.h>
#include <secretary/test/project.h>
#include <secretary/test/list.h>
#include <secretary/test/optimization_requisites.h>

#include <CuTest.h>


void RunAllTests(void) 
{
    CuString *output = CuStringNew();
    CuSuite* suite = CuSuiteNew();

    CuSuiteAddSuite(suite, test_project_suite());
    CuSuiteAddSuite(suite, test_task_suite());
    CuSuiteAddSuite(suite, test_secretary_suite());
    CuSuiteAddSuite(suite, test_notebook_suite());    
    CuSuiteAddSuite(suite, test_list_suite());    
    CuSuiteAddSuite(suite, test_optimization_requisites_suite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
    CuStringDelete(output);
    CuSuiteDelete(suite);
}

int main(void)
{
    RunAllTests();
    return 0;
}

