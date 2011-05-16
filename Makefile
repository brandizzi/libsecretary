vpath %.h include/secretary cutest
vpath %.c src cutest

CFLAGS=${_CFLAGS} -Wall -std=c99
CPPFLAGS=-Iinclude -Icutest

LIBOBJS=secretary.o project.o task.o notebook.o parser.o util.o list.o
TESTOBJS=test_secretary.o test_notebook.o test_task.o test_project.o test_list.o test_optimization_requisites.o CuTest.o

TASK_DEPENDENCY_HEADERS=task.h util.h
PROJECT_DEPENDENCY_HEADERS=project.h list.h ${TASK_DEPENDENCY_HEADERS}
SECRETARY_DEPENDENCY_HEADERS=secretary.h ${PROJECT_DEPENDENCY_HEADERS}
NOTEBOOK_DEPENDENCY_HEADERS=notebook.h ${SECRETARY_DEPENDENCY_HEADERS}

LIBNAME=libsecretary.a
DYNAMIC_LIB_OPTION=-dynamiclib
VERSION=0.1

all: run_all ${LIBNAME}

${LIBNAME}: ${LIBOBJS}
	ar rcs $@ ${LIBOBJS} 

test_notebook.o: test/notebook.c test/notebook.h ${NOTEBOOK_DEPENDENCY_HEADERS}
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@
test_secretary.o: test/secretary.c test/secretary.h ${SECRETARY_DEPENDENCY_HEADERS}
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@
test_project.o: test/project.c test/project.h ${PROJECT_DEPENDENCY_HEADERS}
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@
test_task.o: test/task.c test/task.h ${TASK_DEPENDENCY_HEADERS}
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@
test_list.o: test/list.c test/list.h list.h
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@
test_optimization_requisites.o: test/optimization_requisites.c test/optimization_requisites.h \
		${SECRETARY_DEPENDENCY_HEADERS}
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@
	
%.o: %.c %.h
#	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@


run_all.o: test/run_all.c test/secretary.h
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@

run_all: ${TESTOBJS} ${LIBNAME}
test: run_all
	./run_all
clean:
	-rm *.o
