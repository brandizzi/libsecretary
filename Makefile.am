ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES = libsecretary-@API_VERSION@.la
libsecretary_@API_VERSION@_la_SOURCES = list.c notebook.c parser.c project.c secretary.c task.c util.c pubsub.c \
		secretary/list.h \
		secretary/notebook.h secretary/parser.h secretary/project.h secretary/secretary.h secretary/task.h \
		secretary/util.h secretary/_internal/secretary.h secretary/_internal/task.h  secretary/_internal/project.h
libsecretary_@API_VERSION@_la_includedir = $(includedir)/secretary-@API_VERSION@/secretary
libsecretary_@API_VERSION@_la_include_HEADERS = secretary/list.h secretary/notebook.h secretary/parser.h secretary/project.h\
		 secretary/secretary.h secretary/task.h secretary/util.h

libsecretary_@API_VERSION@_la_LDFLAGS = -version-info @INTERFACE_VERSION@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libsecretary-@API_VERSION@.pc

noinst_PROGRAMS = runtests
runtests_SOURCES = test/runtests.c test/test_list.c test/test_notebook.c test/test_optimization_requisites.c\
		test/test_parser.c test/test_project.c test/test_secretary.c test/test_task.c test/test_util.c\
		test/test_pubsub.c CuTest.c CuTest.h secretary/test/list.h secretary/test/notebook.h secretary/test/optimization_requisites.h \
		secretary/test/parser.h secretary/test/project.h secretary/test/secretary.h secretary/test/task.h \
		secretary/test/pubsub.h secretary/test/util.h

runtests_LDADD = libsecretary-@API_VERSION@.la
runtests_LDFLAGS = -static
    

TESTS = runtests

test: check


