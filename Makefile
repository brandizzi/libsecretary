vpath %.h include cutest
vpath %.c src cutest

CFLAGS=-Wall
CPPFLAGS=-Iinclude -Icutest
all: test
secretary.o: secretary.c secretary.h task.h

task.o: task.c task.h

CuTest.o: CuTest.c CuTest.h


test_secretary.o: test/secretary.c test/secretary.h secretary.h
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@

run_all.o: test/run_all.c test/secretary.h
	${CC} -c ${CFLAGS} ${CPPFLAGS} $< -o $@

run_all: run_all.o CuTest.o secretary.o task.o test_secretary.o
test: run_all
clean:
	-rm *.o
