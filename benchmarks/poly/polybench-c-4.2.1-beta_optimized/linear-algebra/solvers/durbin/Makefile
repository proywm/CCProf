include ../../../config.mk

EXTRA_FLAGS=

durbin: durbin.c durbin.h
	${VERBOSE} ${CC} -o durbin durbin.c ${CFLAGS} -I. -I../../../utilities ../../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f durbin

