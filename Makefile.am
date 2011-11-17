lib_LIBRARIES = libsecretary.a
libsecretary_a_SOURCES = list.c notebook.c parser.c project.c secretary.c task.c util.c

noinst_PROGRAMS = runtests
runtests_SOURCES = test/runtests.c test/test_list.c test/test_notebook.c test/test_optimization_requisites.c\
		test/test_parser.c test/test_project.c test/test_secretary.c test/test_task.c test/test_util.c\
		CuTest.c 
runtests_LDADD = libsecretary.a

TESTS = runtests

test: check
