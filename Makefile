vpath %.h include/secretary cutest
vpath %.c src cutest

CFLAGS=${_CFLAGS} -Wall -std=c99
CPPFLAGS=-Iinclude -Icutest

LIBOBJS=secretary.o project.o task.o notebook.o parser.o util.o list.o
TESTOBJS=test_secretary.o test_notebook.o test_task.o test_project.o test_list.o CuTest.o

LIBNAME=libsecretary.a
DYNAMIC_LIB_OPTION=-dynamiclib
VERSION=0.1

all: run_all ${LIBNAME}

${LIBNAME}: ${LIBOBJS}
	ar rcs $@ ${LIBOBJS} 

test_%.o: test/%.c test/%.h %.h
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
