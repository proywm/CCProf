include ../../../config.mk

EXTRA_FLAGS=-lm

cholesky: cholesky.c cholesky.h
	${VERBOSE} ${CC} -o cholesky cholesky.c ${CFLAGS} -I. -I../../../utilities ../../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f cholesky

