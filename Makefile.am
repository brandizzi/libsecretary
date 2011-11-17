LIB_VERSION=0:1:2
ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libsecretary.la
libsecretary_la_SOURCES = list.c notebook.c parser.c project.c secretary.c task.c util.c secretary/list.h \
		secretary/notebook.h secretary/parser.h secretary/project.h secretary/secretary.h secretary/task.h \
		secretary/util.h secretary/_internal/secretary.h secretary/_internal/task.h

libsecretary_la_LDFLAGS = -version-info $(LIB_VERSION)

noinst_PROGRAMS = runtests
runtests_SOURCES = test/runtests.c test/test_list.c test/test_notebook.c test/test_optimization_requisites.c\
		test/test_parser.c test/test_project.c test/test_secretary.c test/test_task.c test/test_util.c\
		CuTest.c CuTest.h secretary/test/list.h secretary/test/notebook.h secretary/test/optimization_requisites.h \
		secretary/test/parser.h secretary/test/project.h secretary/test/secretary.h secretary/test/task.h \
		secretary/test/util.h

runtests_LDADD = libsecretary.la

TESTS = runtests

test: check


