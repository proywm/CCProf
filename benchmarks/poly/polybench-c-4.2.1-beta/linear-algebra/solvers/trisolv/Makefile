include ../../../config.mk

EXTRA_FLAGS=

trisolv: trisolv.c trisolv.h
	${VERBOSE} ${CC} -o trisolv trisolv.c ${CFLAGS} -I. -I../../../utilities ../../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f trisolv

