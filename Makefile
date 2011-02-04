vpath %.h include cutest
vpath %.c src cutest

CFLAGS=-Wall -std=c99
CPPFLAGS=-Iinclude -Icutest
LIBOBJS=secretary.o project.o task.o
all: test
secretary.o: secretary.c secretary.h task.h project.h
task.o: task.c task.h
project.o: project.c project.h task.h
CuTest.o: CuTest.c CuTest.h


test_secretary.o: test/secretary.c test/secretary.h secretary.h
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@

run_all.o: test/run_all.c test/secretary.h
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@

run_all: run_all.o test_secretary.o CuTest.o ${LIBOBJS} 
test: run_all
clean:
	-rm *.o
