vpath %.h include cutest
vpath %.c src cutest

CFLAGS=${_CFLAGS} -Wall -std=c99
CPPFLAGS=-Iinclude -Icutest
LIBOBJS=secretary.o project.o task.o
LIBNAME=libsecretary.dylib
DYNAMIC_LIB_OPTION=-dynamiclib
VERSION=0.1
all: test ${LIBNAME}

${LIBNAME}: ${LIBOBJS}
	${CC} ${DYNAMIC_LIB_OPTION} -current_version ${VERSION} ${CFLAGS} -shared -o $@ ${LIBOBJS} 
secretary.o: secretary.c secretary.h task.h project.h
task.o: task.c task.h
project.o: project.c project.h task.h
CuTest.o: CuTest.c CuTest.h


test_secretary.o: test/secretary.c test/secretary.h secretary.h
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@

run_all.o: test/run_all.c test/secretary.h
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@

run_all: run_all.o test_secretary.o CuTest.o ${LIBNAME} 
test: run_all
clean:
	-rm *.o
