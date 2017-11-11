include ../../../config.mk

EXTRA_FLAGS=

gemver: gemver.c gemver.h
	${VERBOSE} ${CC} -o gemver gemver.c ${CFLAGS} -I. -I../../../utilities ../../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f gemver

