include ../../../config.mk

EXTRA_FLAGS=

3mm: 3mm.c 3mm.h
	${VERBOSE} ${CC} -o 3mm 3mm.c ${CFLAGS} -I. -I../../../utilities ../../../utilities/polybench.c ${EXTRA_FLAGS}

clean:
	@ rm -f 3mm

