EXECS=main
MPICC?=mpicc

CFLAGS= -g -Wall

${EXECS}: ${EXECS}.c
	${MPICC} ${CFLAGS} -o ${EXECS} ${EXECS}.c

clean:
	rm ${EXECS}
